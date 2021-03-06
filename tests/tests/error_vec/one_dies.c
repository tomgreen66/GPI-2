#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <test_utils.h>

#define _4MB 4194304
//simulate the case that one dies but we want/can continue


//recover => clean the queue
gaspi_return_t recover()
{
  gaspi_return_t ret = GASPI_ERROR;

  gaspi_printf("Enter recover\n");
  while(ret != GASPI_SUCCESS)
    {
      ret = gaspi_wait(0, GASPI_BLOCK);
    }

  gaspi_printf("Exit recover with %d\n", ret);
  return ret;
}

int main(int argc, char *argv[])
{
  gaspi_rank_t nprocs, myrank, i;
  int j, n;
  gaspi_rank_t *avoid_list;
  gaspi_group_t survivors;

  TSUITE_INIT(argc, argv);

  ASSERT (gaspi_proc_init(GASPI_BLOCK));
  ASSERT(gaspi_proc_num(&nprocs));
  ASSERT(gaspi_proc_rank(&myrank));

  ASSERT(gaspi_segment_create(0, 
			      _4MB,
			      GASPI_GROUP_ALL, 
			      GASPI_BLOCK, 
			      GASPI_MEM_INITIALIZED));


  avoid_list = (gaspi_rank_t *) malloc(nprocs * sizeof(gaspi_rank_t));

  assert (avoid_list != NULL);
  memset(avoid_list, 0, nprocs * sizeof(gaspi_rank_t));

  gaspi_state_vector_t vec = (gaspi_state_vector_t) malloc(nprocs);

  ASSERT(gaspi_state_vec_get(vec));

  //check that everyone is healthy
  for(i = 0; i < nprocs; i++)
    {
      assert(vec[i] == 0);
    }

  //sync
  ASSERT (gaspi_barrier(GASPI_GROUP_ALL, GASPI_BLOCK));

  //now last rank dies
  if(myrank == nprocs - 1)
    exit(-1);

  else
    {
      //create group of survivors 
      ASSERT(gaspi_group_create(&survivors));
      for(i = 0; i < nprocs - 1; i++)
	ASSERT(gaspi_group_add(survivors, i));
      
      ASSERT(gaspi_group_commit(survivors, GASPI_BLOCK));
      
      gaspi_printf("Done with groups\n");
      
      sleep(2);
    }
  //the others communicate
  gaspi_return_t retval;

  for(j = 0; j < 10; j++)
    {
      gaspi_printf("Iteration %d\n", j);
      for(i = 0; i < nprocs; i++)
	{
	  if( avoid_list[i] != 1 )
	    ASSERT(gaspi_write(0, 0, i,
			       0, 0, sizeof(int),
			       0,
			       GASPI_BLOCK));
	  
	  
	}
      retval = gaspi_wait(0, GASPI_BLOCK);
      
      //problem found -> recover
      if(retval != GASPI_SUCCESS)
	{
	  ASSERT(gaspi_state_vec_get(vec));
	  for(n = 0; n < nprocs; n++)
	    {
	      if(vec[n] != GASPI_STATE_HEALTHY)
		{
		  gaspi_printf("Problem with node %d detected\n", n);
		  assert(n == (nprocs - 1));
		  
		  ASSERT(recover());
		  
		  avoid_list[n] = 1;
		}
	    }
	}
    }	  

  ASSERT (gaspi_barrier(survivors, GASPI_BLOCK));

  ASSERT (gaspi_proc_term(GASPI_BLOCK));
  
  gaspi_printf("exiting\n");
  return EXIT_SUCCESS;
}


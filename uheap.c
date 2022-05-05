#include <inc/lib.h>
#define arrSize (USER_HEAP_MAX - USER_HEAP_START)/4096

// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.
struct HeapArray{
	int Page_status ;
	uint32 Chain_length ;

};

uint32 FreeSize=0;
int initalizing =0 ;
int numOfbusyBlocks =0;

struct HeapArray Array [arrSize] ={};

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
void* malloc(uint32 size)
{
	//TODO: [FINAL_EVAL_2020 - VER_C] - [2] USER HEAP [User Side malloc]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	//refer to the documentation for details
	if(size>(USER_HEAP_MAX - USER_HEAP_START))
			return NULL;
	FreeSize=size;
	int numOfBlocksNeeded = (ROUNDUP(size,4096))/4096;
	int its_Allocated = 0 ;
	int startVIndex = 0;
	uint32 Va ;
	int failed=0;
	int FreeBlocks = arrSize - numOfbusyBlocks ;

	if(numOfBlocksNeeded>FreeBlocks)
		return NULL;

	//intializing the array :
	if(initalizing==0){
		for(int i=0;i<arrSize;i++)
		{
			Array[i].Page_status=0; //means its free 1 is allocated
			Array[i].Chain_length = 0 ; // returns the length of the block chain


		}
		initalizing++;
	}



	//This function should find the space of the required range by either:
	//1) FIRST FIT strategy
	//2) BEST FIT strategy

	if(sys_isUHeapPlacementStrategyFIRSTFIT())
	{

		for(int i=0 ; i<arrSize;i++)
		{
			int j=i;
			if(Array[i].Page_status==0)
			{

				for(j = i ;j<(i+numOfBlocksNeeded) ; j++)
				{
					if(Array[j].Page_status!=0)
					{
						i=j ;
						break;
					}
				}

				if(i!= j)
				{
					startVIndex=i;
				for(j = i ;j<(i+numOfBlocksNeeded) ; j++)
					{
						Array[j].Page_status=1;
						//Array[j].Chain_length +=1;
						numOfbusyBlocks+=1;
					}
					its_Allocated=1;
				}



			}

			if(its_Allocated==1)
			   break;

			failed= i ;
		}

		if(failed+1==arrSize)
			return NULL;

		else{
					Array[startVIndex].Chain_length=numOfBlocksNeeded;
					//cprintf("chain len: %d\n",Array[startVIndex].Chain_length);
					Va=(4096*startVIndex)+USER_HEAP_START;
					sys_allocateMem(Va,size);

		    }


	}


	else if(sys_isUHeapPlacementStrategyBESTFIT())

	{
		int mini = arrSize;
			int temp = 0 ;
			int len=0;


/*
		if(FreeBlocks == arrSize)
		{
			for(int i=0 ; i<arrSize;i++)
					{
						int j=i;
						if(Array[i].Page_status==0)
						{

							for(j = i ;j<(i+numOfBlocksNeeded) ; j++)
							{
								if(Array[j].Page_status!=0)
								{
									i=j ;
									break;
								}
							}

							if(i!= j)
							{
								startVIndex=i;
							for(j = i ;j<(i+numOfBlocksNeeded) ; j++)
								{
									Array[j].Page_status=1;
									//Array[j].Chain_length +=1;
									numOfbusyBlocks+=1;
								}
								its_Allocated=1;
							}



						}

						if(its_Allocated==1)
						   break;

						failed= i ;
					}

					if(failed+1==arrSize)
						return NULL;

					else{
								Array[startVIndex].Chain_length=numOfBlocksNeeded;
								//cprintf("chain len: %d\n",Array[startVIndex].Chain_length);
								Va=(4096*startVIndex)+USER_HEAP_START;
								sys_allocateMem(Va,size);

					    }



		}
*/
		//if {

			for(int i=0 ; i<arrSize ; i++)
			{
				/*
				if(Array[i].Page_status==0)
				{
					for(int j=i ;j<arrSize;j++)
					{
						if(Array[j].Page_status==0)
						{
							temp=i;
							len+=1;
						}
						else{
							i=j;
							break;
						}

					}
*/
					if(len>=numOfBlocksNeeded && len<mini)
					{
						mini=len;
						startVIndex=temp;

					}

				}



			Array[startVIndex].Chain_length=numOfBlocksNeeded;
			//cprintf("chain len: %d\n",Array[startVIndex].Chain_length);
			Va=(4096*startVIndex)+USER_HEAP_START;
			cprintf("hello from bestfit\n");

			sys_allocateMem(Va,size);



	}

	return (uint32 *) Va;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [FINAL_EVAL_2020 - VER_C] - [2] USER HEAP [User Side free]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");

	int StartIndex = (int)(virtual_address - USER_HEAP_START )/4096;
	uint32 va=(uint32)virtual_address;


			for(int i =StartIndex ; i<((Array[StartIndex].Chain_length)+StartIndex);i++)
			{
				Array[i].Page_status = 0 ;

			}


			sys_freeMem((uint32) virtual_address,Array[StartIndex].Chain_length);

	//you should get the size of the given allocation using its address

	//refer to the documentation for details
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//
//===============
// [1] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	panic("this function is not required...!!");
	return 0;
}


//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("this function is not required...!!");
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("this function is not required...!!");
	return 0;
}

void sfree(void* virtual_address)
{
	panic("this function is not required...!!");
}

void expand(uint32 newSize)
{
	panic("this function is not required...!!");
}
void shrink(uint32 newSize)
{
	panic("this function is not required...!!");
}

void freeHeap(void* virtual_address)
{
	panic("this function is not required...!!");
}

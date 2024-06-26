#include "table.h"
#include "system.h"

//----------------------------------------------------------PTable----------------------------------------------------------

PTable::PTable(int size)
{
	int i = 0;
	psize = size;
	bm = new BitMap(size);
	bmsem = new Semaphore("BMsem",1);
		
	for(i = 0 ; i < MAXPROCESS ; ++i)
		pcb[i] = NULL;

	pcb[0] = new PCB(0);
	currentThread = new Thread("main");
	bm->Mark(0);
}

PTable::~PTable()
{
	int i=0;
	if(bm!=NULL)
		delete bm;
	if(bmsem!=NULL)
		delete bmsem;
	
    //Delete this thing will cause segmentation fault (core dumped)
    //Since when we delete processTab, we will have core dumped and there will be memory leak
        //we will comment it to prevent the delete error
		//there will be not too much memory leak since we only have 10 process
		//and we won't rerun nachos for many times in one session of virtual machine
    //For debugging: error occured when delete exitsem of pcb[1], for more information, see ~PCB()
	// for(i=0; i<MAXPROCESS; i++) {
	// 	if(pcb[i]!=NULL) {
	// 		delete pcb[i];
	// 	}
	// }
}

//--------------------------------------------------------------------

int PTable::ExecUpdate(char* filename)
{
	bmsem->P();			//chi nap 1 tien trinh vao mot thoi diem

//Kiem tra file co ton tai tren may khong
	OpenFile *executable = fileSystem->Open(filename);
	if (executable == NULL) 
	{
		printf("\nUnable to open file %s\n", filename);
		bmsem->V();
		return -1;
    	}
	delete executable;			// close file
////////////////////////////////////////////////////////////

//Kiem tra chuong trinh duoc goi co la chinh no khong
	if(!strcmp(filename,currentThread->getName()))
	{
		printf("\nLoi: khong duoc phep goi exce chinh no !!!\n");
		bmsem->V();
		return -1;
	}
////////////////////////////////////////////////////////////

//Kiem tra con slot trong khong
	int ID= GetFreeSlot();
	if(ID==-1)
	{
		printf("\nLoi: Da vuot qua 10 tien trinh !!!\n");
		bmsem->V();
		return -1;
	}
////////////////////////////////////////////////////////////

	pcb[ID]= new PCB(ID);
	bm->Mark(ID);
	int pID= pcb[ID]->Exec(filename,ID);

	bmsem->V();
	return pID;
}

int PTable::ExitUpdate(int ec)
{
//Kiem tra pID co ton tai khong
	int pID= currentThread->GetProcessID();
	if(!IsExist(pID))
	{
		printf("\nLoi: Tien trinh khong ton tai !!!\n");
		return -1;
	}
//////////////////////////////////////////////////////////////

//Neu la main process thi Halt()
	if(pID==0)
	{
		interrupt->Halt();
		return 0;
	}
/////////////////////////////////////////////////////////////

	pcb[pID]->SetExitCode(ec);
	
	if(pcb[pID]->JoinStatus != -1)
	{
		pcb[pID]->JoinRelease();
		pcb[pID]->ExitWait();
		Remove(pID);	
	}
	else
		Remove(pID);
	return ec;
}

int PTable::JoinUpdate(int pID)
{
	
	if(pID <= 0 || pID > 9)
	{
		printf("\nLoi: Khong ton tai process: id = %d\n",pID);
		return -1;
	}

	if (pcb[pID] == NULL)
	{
		printf("Loi: Khong ton tai process id nay!");
		return -1;
	}

//kiem tra tien trinh dang chay co la cha cua tien trinh can join hay khong
	if(currentThread->GetProcessID() != pcb[pID]->parentID)
	{
		printf("\nLoi: Ko duoc phep join vao tien trinh khong phai cha cua no !!!\n");
		return -1;
	}
/////////////////////////////////////////////////////////////////////////////////////////////
	

	pcb[pID]->JoinWait(); 	//doi den khi tien trinh con ket thuc

	int ec = pcb[pID]->GetExitCode();

	if(ec != 0)
	{
		printf("\nProcess exit with exitcode EC = %d ",ec);
		return -1;
	}

	pcb[pID]->ExitRelease();	//cho phep tien trinh con ket thuc
	
	return 0;
}

void PTable::Remove(int pID)
{
	if(pID<0 || pID>9)
		return;
	if(bm->Test(pID))
	{
		bm->Clear(pID);
		delete pcb[pID];
	}
}

//----------------------------------------------------------------------------------------------
int PTable::GetFreeSlot()
{
	return bm->FindFreeSlot();
}

bool PTable::IsExist(int pID)
{
	if(pID<0 || pID>9)
		return 0;
	return bm->Test(pID);
}

char* PTable::GetName(int pID)
{
	if(pID>=0 && pID<10 && bm->Test(pID))
		return pcb[pID]->GetNameThread();
}

//----------------------------------------------------------STable----------------------------------------------------------

STable::STable()
{
	int i = 0;
	bm = new BitMap(MAX_SEMAPHORES);
	for (i = 0 ; i < MAX_SEMAPHORES; i++) {
		semTab[i] = NULL;
	}
}

STable::~STable()
{
	int i = 0;
	if (bm != NULL) {
		delete bm;
	}
	for (i = 0; i < MAX_SEMAPHORES; i++) {
		if (semTab[i] != NULL) {
			delete semTab[i];
		}
	}
}

int STable::Create(char* name, int init)
{
	for (int i = 0; i < MAX_SEMAPHORES; i++)
	{
		if (bm->Test(i) && !strcmp(semTab[i]->getName(), name))
		{
			printf("\nLoi: Semaphore da ton tai !!!\n");
			return -1;
		}
	}
	int ID = GetFreeSlot();
	if (ID == -1)
	{
		printf("\nLoi: Da vuot qua so luong semaphore toi da !!!\n");
		return -1;
	}
	semTab[ID]= new Semaphore(name, init);
	bm->Mark(ID);
	return ID;
}

int STable::Wait(char* name)
{
	int ID = -1;
	for (int i = 0; i < MAX_SEMAPHORES; i++)
		if (semTab[i] != NULL && !strcmp(semTab[i]->getName(), name))
		{
			ID = i;
			break;
		}
	if (ID == -1)
	{
		printf("\nLoi: Khong ton tai semaphore nay !!!\n");
		return -1;
	}
	semTab[ID]->P();
	return 0;
}

int STable::Signal(char* name)
{
	int ID = -1;
	for (int i = 0; i < MAX_SEMAPHORES; i++)
		if (semTab[i] != NULL && !strcmp(semTab[i]->getName(),name))
		{
			ID = i;
			break;
		}
	if(ID == -1)
	{
		printf("\nLoi: Khong ton tai semaphore nay !!!\n");
		return -1;
	}
	semTab[ID]->V();
	return 0;
}

int STable::GetFreeSlot()
{
	return bm->FindFreeSlot();
}

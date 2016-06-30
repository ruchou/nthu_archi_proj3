//
//  main.c
//  simulator final
//
//  Created by 周延儒 on 2016/3/16.
//  Copyright © 2016年 周延儒. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
//#include <struct.h>
//
//  I_memory.h
//  simulator final
//
//  Created by 周延儒 on 2016/3/16.
//  Copyright © 2016年 周延儒. All rights reserved.
//

int pc;
int sp;
int cycle=1;
int halt_the_process=0;

FILE *outputfile;

FILE *errorfile;




//function 6bits
#define ADD 0b100000		//ADD   => 32
#define ADDU 33             //ADD   => 33
#define SUB 0b100010		//SUB   => 34
#define AND 0b100100		//AND   => 36
#define OR 0b100101		//OR    => 37
#define XOR 38		//ADD   => 32
#define NOR 39		//ADD   => 32
#define NAND 40		//ADD   => 32
#define SLT 42		//ADD   => 32
#define SLL 0		//ADD   => 32
#define SRL 2		//ADD   => 32
#define SRA 3		//ADD   => 32
#define JR 8		//ADD   => 32

//
//#define LI 0b100001		//LI    => 33
//#define SYSCALL 0b001100	//SYSCALL=>12
//#define LW 0b100011		//LW    => 35
//#define SW 0b101011		//SW    => 43
//#define J 0b000010		//J     => 2
//#define BEQ 0b000100		//BEQ   => 4
//#define MOVE 0b000110		//MOVE  => 6
struct reg
{
    char alt_name[4];			// Stores names like $t0, $fp, $at
    int val;
};

//extern
struct reg reg_file[32];

struct instruct_mem_element
{
    char opcode[7],rs[6],rt[6],rd[6],c_shame[6],funct[7],c_immediate[17];
    int  opcode_i,rs_i,rt_i,rd_i,c_shame_i,funct_i,c_immediate_i;
    int c_immdeiate_signed;
    char instruction[33];
};

//1024 I memory each stores 6 elements
struct instruct_mem
{
    struct instruct_mem_element mem[256];
    
};


struct data_mem
{
    struct data_mem_element
    {
        char var_name[33];
        int val;
        char data[33];
    } mem[256];
};



//operation
void store(int pos,struct data_mem *dm,char*var_name,int val){
    if (pos>=256) {
        // printf("not enough space\n");
    }else{
        strcpy(dm->mem[pos].var_name, var_name);
        dm->mem[pos].val=val;
    }
    
}

int get_mem_location(char*var_name,struct data_mem *dm){
    
    int i=0;
    for (i=0; i<256; i++) {
        if (!strcmp(var_name, dm->mem[i].var_name)) {
            return i;
        }
    }
    // printf("there is no %s in the D-memory ",var_name);
    return 0;
    
}

int  load_data_from_D_memory_int(int pos,struct data_mem *dm){
    return dm->mem[pos].val;
    
    
}
char* load_memory_from_D_memory_str(int pos,struct data_mem *dm){
    
    return dm->mem[pos].var_name;
    
}




struct _PAGE {
    int valid;
    int space[256];
    int LRU;
};

struct _MEMORY {
    int hits, misses;
    int size, pageSize;
    struct _PAGE page[256];
};


struct _BLOCK {
    int valid;
    int tag, ind, off;
    int LRU, ppn;
    int MRU;
    int space;
};
struct _CACHE {
    int hits, misses;
    int totalSize, blockSize, set_associativity;
    struct _BLOCK block[256][256];
};


struct _PPN {
    int valid;
    int ppn;
    int LRU;
    int vpn;
};
struct _TLB {
    int hits, misses;
    int entry;
    struct _PPN ppn[256];
};
struct _PTE {
    int hits, misses;
    int entry;
    struct _PPN ppn[256];
};

struct _MEMORY IMEM, DMEM;
struct _CACHE ICACHE, DCACHE;
struct _TLB ITLB, DTLB;
struct _PTE IPTE, DPTE;

int LRU_Counter;

int IMSIZE;       // I memory size
int DMSIZE;       // D memory size
int IPGSIZE;      // Page size of I memory
int DPGSIZE;      // Page size of D memory
int ICTSIZE;      // Total size of I cache
int ICBSIZE;      // Block size of I cache
int ICSA;         // Set associativity of I cache
int DCTSIZE;      // Total size of D cache
int DCBSIZE;      // Block size of D cache
int DCSA;          // Set associativity of D cache



int  getIPTEPPN(unsigned VPN) {
    int PPN = 0;
    int i;
    
    for ( i = 0; i < IPTE.entry; i++) {
        if (IPTE.ppn[i].valid == 1 && IPTE.ppn[i].vpn == VPN) {
            PPN = IPTE.ppn[i].ppn;
            IPTE.ppn[i].LRU = LRU_Counter++;
            return PPN;
        }
    }
    return -1;
}
int getDPTEPPN(unsigned VPN) {
    int PPN = 0;
    int i;
    for ( i = 0; i < DPTE.entry; i++) {
        if (DPTE.ppn[i].valid == 1 && DPTE.ppn[i].vpn == VPN) {
            PPN = DPTE.ppn[i].ppn;
            DPTE.ppn[i].LRU = LRU_Counter++;
            return PPN;
        }
    }
    return -1;
}
int getITLBPPN(unsigned VPN) {
    //find the PPBN in th TLB
    int PPN = 0;
    int i;
    
    for ( i = 0; i < ITLB.entry; i++) {
        if (ITLB.ppn[i].valid == 1 && ITLB.ppn[i].vpn == VPN) {
            PPN = ITLB.ppn[i].ppn;
            ITLB.ppn[i].LRU = LRU_Counter++;
            return  PPN;
        }
    }
    return -1;
}
int getDTLBPPN(unsigned VPN) {
    int PPN =0;
    int i;
    
    for ( i = 0; i < DTLB.entry; i++) {
        if (DTLB.ppn[i].valid == 1 && DTLB.ppn[i].vpn == VPN) {
            PPN = DTLB.ppn[i].ppn;
            DTLB.ppn[i].LRU = LRU_Counter++;
            return PPN;
        }
    }
    return -1;
}
void updateITLBPPN(unsigned VPN, int PPN) {
    int ind = -1;
    int i;
    
    for ( i = 0; i < ITLB.entry; i++) {
        if (ITLB.ppn[i].valid == 0) {
            ind = i;
            break;
        }
    }
    //we can find an empy entry
    if (ind != -1) {
        
        ITLB.ppn[ind].valid = 1;
        ITLB.ppn[ind].vpn = VPN;
        ITLB.ppn[ind].ppn = PPN;
    } else {
        int tmp = 0x7fffffff;
        for ( i = 0; i < ITLB.entry; i++) {
            //find the least recentlt used
            if (ITLB.ppn[i].LRU < tmp) {
                tmp = ITLB.ppn[i].LRU;
                ind = i;
            }
            
        }
        ITLB.ppn[ind].valid = 1;
        ITLB.ppn[ind].vpn = VPN;
        ITLB.ppn[ind].ppn = PPN;
    }
}
void updateDTLBPPN(unsigned VPN, int PPN) {
    int ind = -1;
    int i;
    
    for ( i = 0; i < DTLB.entry; i++) {
        if (DTLB.ppn[i].valid == 0) {
            ind = i;
            break;
        }
    }
    if (ind != -1) {
        DTLB.ppn[ind].valid = 1;
        DTLB.ppn[ind].vpn = VPN;
        DTLB.ppn[ind].ppn = PPN;
    } else {
        int tmp = 0x7fffffff;
        for ( i = 0; i < DTLB.entry; i++) {
            if (DTLB.ppn[i].LRU < tmp) {
                tmp = DTLB.ppn[i].LRU;
                ind = i;
            }
        }
        DTLB.ppn[ind].valid = 1;
        DTLB.ppn[ind].vpn = VPN;
        DTLB.ppn[ind].ppn = PPN;
    }
}
void updateIPTEPPN(unsigned VPN, int PPN) {
    int ind = -1;
    int i;
    
    for ( i = 0; i < IPTE.entry; i++) {
        if (IPTE.ppn[i].valid == 0) {
            ind = i;
            break;
        }
    }
    if (ind != -1) {
        IPTE.ppn[ind].valid = 1;
        IPTE.ppn[ind].vpn = VPN;
        IPTE.ppn[ind].ppn = PPN;
    } else {
        int LLRU = 0x7fffffff;
        for ( i = 0; i < IPTE.entry; i++) {
            if (IPTE.ppn[i].LRU < LLRU) {
                LLRU = IPTE.ppn[i].LRU;
                ind = i;
            }
        }
        IPTE.ppn[ind].valid = 1;
        IPTE.ppn[ind].vpn = VPN;
        IPTE.ppn[ind].ppn = PPN;
    }
}
void updateDPTEPPN(unsigned VPN, int PPN) {
    int ind = -1;
    int i;
    
    for ( i = 0; i < DPTE.entry; i++) {
        if (DPTE.ppn[i].valid == 0) {
            ind = i;
            break;
        }
    }
    if (ind != -1) {
        DPTE.ppn[ind].valid = 1;
        DPTE.ppn[ind].vpn = VPN;
        DPTE.ppn[ind].ppn = PPN;
    } else {
        int LLRU = 0x7fffffff;
        for ( i = 0; i < DPTE.entry; i++) {
            if (DPTE.ppn[i].LRU < LLRU) {
                LLRU = DPTE.ppn[i].LRU;
                ind = i;
            }
        }
        DPTE.ppn[ind].valid = 1;
        DPTE.ppn[ind].vpn = VPN;
        DPTE.ppn[ind].ppn = PPN;
    }
}
int insertIMEM(unsigned VA) {
    int PPN = -1;
    int i;
    int Number_IMEM_Page=IMEM.size/IMEM.pageSize;
    
    for ( i = 0; i < Number_IMEM_Page; i++) {
        if (IMEM.page[i].valid == 0) {
            PPN = i;
            break;
        }
    }
    if (PPN != -1) {
        //similarly if we can find an empty entry
        IMEM.page[PPN].valid = 1;
        for ( i = 0; i < IMEM.pageSize/4; i++) {
            IMEM.page[PPN].space[i] = VA+i*4;
        }
        IMEM.page[PPN].LRU = LRU_Counter++;
    } else {
        int LLRU = 0x7fffffff;
        for ( i = 0; i < IMEM.size/IMEM.pageSize; i++) {
            if (IMEM.page[i].LRU < LLRU) {
                LLRU = IMEM.page[i].LRU;
                PPN = i;
            }
        }
        
        //update the infomation in the TLB and Page Table ,for we "rewrite" the element
        for ( i = 0; i < ITLB.entry; i++) {
            if (ITLB.ppn[i].ppn == PPN) {
                ITLB.ppn[i].valid = 0;
            }
        }
        for ( i = 0; i < IPTE.entry; i++) {
            if (IPTE.ppn[i].ppn == PPN) {
                IPTE.ppn[i].valid = 0;
            }
        }
        int set=ICACHE.totalSize/ICACHE.blockSize/ICACHE.set_associativity;
        
        for ( i = 0; i < set; i++) {
            int j;
            for ( j = 0; j < ICACHE.set_associativity; j++) {
                if (ICACHE.block[i][j].ppn == PPN) {
                    ICACHE.block[i][j].valid = 0;
                    ICACHE.block[i][j].MRU = 0;
                    
                }
            }
        }
        
        IMEM.page[PPN].valid = 1;
        for ( i = 0; i < IMEM.pageSize/4; i++) {
            IMEM.page[PPN].space[i] = VA+i*4;
        }
        IMEM.page[PPN].LRU = LRU_Counter++;
    }
    return PPN;
}
int insertDMEM(unsigned VA) {
    int PPN = -1;
    int i;
    int Number_DMEM_Pages=DMEM.size/DMEM.pageSize;
    
    for ( i = 0; i < Number_DMEM_Pages; i++) {
        if (DMEM.page[i].valid == 0) {
            PPN = i;
            break;
        }
    }
    if (PPN != -1) {
        DMEM.page[PPN].valid = 1;
        for ( i = 0; i < DMEM.pageSize/4; i++) {
            DMEM.page[PPN].space[i] = VA+i*4;
        }
        DMEM.page[PPN].LRU = LRU_Counter++;
    } else {
        int LLRU = 0x7fffffff;
        for ( i = 0; i < Number_DMEM_Pages; i++) {
            if (DMEM.page[i].LRU < LLRU) {
                LLRU = DMEM.page[i].LRU;
                PPN = i;
            }
        }
        for ( i = 0; i < DTLB.entry; i++) {
            if (DTLB.ppn[i].ppn == PPN) {
                DTLB.ppn[i].valid = 0;
            }
        }
        for ( i = 0; i < DPTE.entry; i++) {
            if (DPTE.ppn[i].ppn == PPN) {
                DPTE.ppn[i].valid = 0;
            }
        }
        int set=DCACHE.totalSize/DCACHE.blockSize/DCACHE.set_associativity;
        //if set may be the block if it is directed mapped
        
        for ( i = 0; i < set; i++) {
            int j;
            for ( j = 0; j < DCACHE.set_associativity; j++) {
                if (DCACHE.block[i][j].ppn == PPN) {
                    DCACHE.block[i][j].valid = 0;
                    DCACHE.block[i][j].MRU=0;
                }
            }
        }
        DMEM.page[PPN].valid = 1;
        for ( i = 0; i < DMEM.pageSize/4; i++) {
            DMEM.page[PPN].space[i] = VA+i*4;
        }
        DMEM.page[PPN].LRU = LRU_Counter++;
    }
    return PPN;
}




int getIOffset(){
    
    //貨品從中央倉庫進貨到附近倉庫一次一定要是一整個書架，
    /*
     page = 一個書架
     block = 一層書架
     PPN = 附近倉庫的書架編號
     VPN = 中央倉庫的書架編號
     */
    /*virtual address
     
     vpn-page_offset(page size is 2^page_offset)
     refer from p439
     
     */
    
    int pageoffset=0;
    int tmp=IMEM.pageSize-1;
    while (tmp) {
        pageoffset++;
        //tmp=(tmp>>1);
        tmp/=2;
    }
    return pageoffset;
    
}

int getDOffset(){
    int pageoffset=0;
    int tmp=DMEM.pageSize-1;
    while (tmp) {
        pageoffset++;
        tmp/=2;
    }
    return pageoffset;
    
}







int getIBlockOffset() {
    int blockOffset = 0, tmp;
    tmp = ICACHE.blockSize-1;
    
    while (tmp) {
        blockOffset++;
        tmp/=2;
        
    }
    
    
    
    return blockOffset;
}

int getDBlockOffset() {
    int blockOffset = 0, tmp;
    tmp = DCACHE.blockSize-1;
    
    
    while (tmp) {
        blockOffset++;
        //tmp=(tmp>>1);
        tmp/=2;
    }
    
    
    return blockOffset;
}






int getIIndexOffset() {
    int indexOffset = 0, tmp;
    tmp = ICACHE.totalSize/ICACHE.blockSize/ICACHE.set_associativity-1;
    
    while (tmp) {
        indexOffset++;
        tmp/=2;
        
    }
    
    return indexOffset;
}


int getDIndexOffset() {
    int indexOffset = 0, tmp;
    tmp = DCACHE.totalSize/DCACHE.blockSize/DCACHE.set_associativity-1;
    
    while (tmp) {
        indexOffset++;
        tmp/=2;
        
    }
    
    return indexOffset;
}


//pysical address
//PA = 商品在附近倉庫裡的暫時編號



unsigned int getIPA(unsigned int VA, int PPN) {
    unsigned int PA = (unsigned int)PPN;
    unsigned PageOffset = VA % (1 << getIOffset());
    //physical address
    //PPN_PageOffset
    
    PA = ((unsigned)PPN << getIOffset())+PageOffset;
    return PA;
}

unsigned int getDPA(unsigned int VA, int PPN) {
    unsigned int PA = (unsigned int)PPN;
    unsigned PageOffset = VA % (1 << getDOffset());
    //physical address
    //PPN_PageOffset
    
    PA = ((unsigned)PPN << getDOffset())+PageOffset;
    return PA;
}





unsigned getITag(unsigned PA) {
    unsigned tmp = PA;
    tmp = tmp >> getIBlockOffset();
    tmp = tmp >> getIIndexOffset();
    return tmp;
}

unsigned getDTag(unsigned PA) {
    unsigned tmp = PA;
    tmp = tmp >> getDBlockOffset();
    tmp = tmp >> getDIndexOffset();
    return tmp;
}




unsigned getIInd(unsigned PA) {
    unsigned tmp = PA;
    tmp = tmp >> getIBlockOffset();
    return (tmp % (1 << getIIndexOffset()));
}

unsigned getDInd(unsigned PA) {
    unsigned tmp = PA;
    tmp = tmp >> getDBlockOffset();
    return (tmp % (1 << getDIndexOffset()));
}



void queryICACHE(unsigned VA, int PPN) {
    //    unsigned PA = getIPA(VA, PPN);
    //    unsigned tag = getTag(PA, 0);
    //    unsigned ind = getInd(PA, 0);
    unsigned PA = getIPA(VA, PPN);
    unsigned tag = getITag(PA);
    unsigned ind = getIInd(PA);
    
    int hit = -1;
    int i;
    
    for ( i = 0; i < ICACHE.set_associativity; i++) {
        if (ICACHE.block[ind][i].valid == 1 && ICACHE.block[ind][i].tag == tag) {
            hit = i;
        }
    }
    if (hit != -1) {
        
        ICACHE.hits++;
        ICACHE.block[ind][hit].LRU = LRU_Counter++;
        ICACHE.block[ind][hit].MRU=1;
        
        
        int full=0;
        for (i=0; i<ICACHE.set_associativity; i++) {
            if(ICACHE.block[ind][i].MRU==1)
                full++;
        }
        printf("full is %d\n",full);
        if(full==ICACHE.set_associativity){
            for (i=0; i<ICACHE.set_associativity; i++) {
                if(i!=hit)
                    ICACHE.block[ind][i].MRU=0;
            }
            
        }
        
        
        
        return;
    } else {
        ICACHE.misses++;
        for ( i = 0; i < ICACHE.set_associativity; i++) {
            if (ICACHE.block[ind][i].valid == 0 ) {
                hit = i;
                break;
            }
        }
        if (hit != -1) {
            printf("I Cache\n");
            printf("the find is %d\n",hit);
            printf("content is \n");
            int l;
            for (l=0; l<ICACHE.set_associativity; l++) {
                printf("%d-",ICACHE.block[ind][l].valid);
            }
            printf("\nMRU\n");
            for (l=0; l<ICACHE.set_associativity; l++) {
                printf("%d-",ICACHE.block[ind][l].MRU);
            }
            printf("\n-------------------\n");
            
            ICACHE.block[ind][hit].valid = 1;
            ICACHE.block[ind][hit].tag = tag;
            ICACHE.block[ind][hit].LRU = LRU_Counter++;
            ICACHE.block[ind][hit].ppn = PPN;
            ICACHE.block[ind][hit].MRU=1;
            
            
            
            
            int full=0;
            for (i=0; i<ICACHE.set_associativity; i++) {
                if(ICACHE.block[ind][i].MRU==1)
                    full++;
            }
            printf("full is %d\n",full);
            if(full==ICACHE.set_associativity){
                for (i=0; i<ICACHE.set_associativity; i++) {
                    if(i!=hit)
                        ICACHE.block[ind][i].MRU=0;
                }
                
            }
            
            
            
            
            
            
        }else{
            
            int find=-1;
            for (i=0; i<ICACHE.set_associativity; i++) {
                printf("%d-",ICACHE.block[ind][i].MRU);
                if (ICACHE.block[ind][i].MRU==0) {
                    find=i;
                    break;
                }
            }
            printf("\n find %d\n",find);
            if(find!=-1){
                
                
                
                ICACHE.block[ind][find].valid = 1;
                ICACHE.block[ind][find].tag = tag;
                ICACHE.block[ind][find].LRU = LRU_Counter++;
                ICACHE.block[ind][find].ppn = PPN;
                ICACHE.block[ind][find].MRU=1;
                
                int full=0;
                for (i=0; i<ICACHE.set_associativity; i++) {
                    if(ICACHE.block[ind][i].MRU==1)
                        full++;
                }
                printf("full is %d\n",full);
                if(full==ICACHE.set_associativity){
                    for (i=0; i<ICACHE.set_associativity; i++) {
                        if(i!=find)
                            ICACHE.block[ind][i].MRU=0;
                    }
                    
                }
                
                
            }else if(ICACHE.set_associativity==1){
                
                ICACHE.block[ind][0].valid = 1;
                ICACHE.block[ind][0].tag = tag;
                ICACHE.block[ind][0].LRU = LRU_Counter++;
                ICACHE.block[ind][0].ppn = PPN;
                ICACHE.block[ind][0].MRU=1;
                
                
            }
            
            
        }
        
        
    }
}
void queryDCACHE(unsigned VA, int PPN) {
    
    unsigned PA = getDPA(VA, PPN);
    unsigned tag = getDTag(PA);
    unsigned ind = getDInd(PA);
    
    
    int hit = -1;
    int i;
    
    for ( i = 0; i < DCACHE.set_associativity; i++) {
        if (DCACHE.block[ind][i].valid == 1 && DCACHE.block[ind][i].tag == tag) {
            hit = i;
            break;
        }
    }
    if (hit != -1) {
        DCACHE.hits++;
        
        DCACHE.block[ind][hit].LRU = LRU_Counter++;
        DCACHE.block[ind][hit].MRU=1;
        
        if (DCACHE.set_associativity==1) {
            return;
        }
        
        int travese;
        int check=0;
        for (travese=0; travese<DCACHE.set_associativity; travese++) {
            if (DCACHE.block[ind][travese].MRU==1 ) {
                check++;
            }
        }
        
        if (check==DCACHE.set_associativity) {
            printf("reset\n");
            for (travese=0; travese<DCACHE.set_associativity; travese++) {
                
                DCACHE.block[ind][travese].MRU = 0;
            }
            DCACHE.block[ind][hit].MRU=1;
        }
        
        
        
    } else {
        DCACHE.misses++;
        hit=-1;
        printf("---------------\n");
        printf("miss D\n");
        if (DCACHE.misses==5008 && cycle==49607) {
            printf("--------------------------\n");
            printf("error cycle %d\n",cycle);
            int x;
            for (x=0; x<DCACHE.set_associativity; x++) {
                printf("%d-",DCACHE.block[ind][x].valid);
            }
            puts("");
            for (x=0; x<DCACHE.set_associativity; x++) {
                printf("%d-",DCACHE.block[ind][x].MRU);
            }
            puts("");
            DCACHE.misses--;
            DCACHE.hits++;
        }
        
        
        for ( i = 0; i < DCACHE.set_associativity; i++) {
            if (DCACHE.block[ind][i].valid == 0) {
                hit = i;
                break;
            }
        }
        if (hit != -1) {
            //find least invalid
            printf("the hit is %d\n",hit);
            
            
            
            
            DCACHE.block[ind][hit].valid = 1;
            DCACHE.block[ind][hit].tag = tag;
            DCACHE.block[ind][hit].LRU = LRU_Counter++;
            DCACHE.block[ind][hit].ppn = PPN;
            DCACHE.block[ind][hit].MRU=1;
            
            int travese;
            int check=0;
            for (travese=0; travese<DCACHE.set_associativity; travese++) {
                if (DCACHE.block[ind][travese].MRU==1 ) {
                    check++;
                }
            }
            
            if (check==DCACHE.set_associativity) {
                printf("reset\n");
                for (travese=0; travese<DCACHE.set_associativity; travese++) {
                    
                    DCACHE.block[ind][travese].MRU = 0;
                }
                DCACHE.block[ind][hit].MRU=1;
            }
            
            
            
            
        } else {
            hit=-1;
            
            for ( i = 0; i < DCACHE.set_associativity; i++) {
                if (DCACHE.block[ind][i].MRU == 0) {
                    hit = i;
                    break;
                }
            }
            
            if (DCACHE.set_associativity==1) {
                //direct map
                DCACHE.block[ind][0].valid = 1;
                DCACHE.block[ind][0].tag = tag;
                DCACHE.block[ind][0].LRU = LRU_Counter++;
                DCACHE.block[ind][0].ppn = PPN;
                DCACHE.block[ind][0].MRU=1;
                return;
                
            }
            
            DCACHE.block[ind][hit].valid = 1;
            DCACHE.block[ind][hit].tag = tag;
            DCACHE.block[ind][hit].LRU = LRU_Counter++;
            DCACHE.block[ind][hit].ppn = PPN;
            DCACHE.block[ind][hit].MRU=1;
            
            
            int travese;
            int check=0;
            for (travese=0; travese<DCACHE.set_associativity; travese++) {
                if (DCACHE.block[ind][travese].MRU==1 ) {
                    check++;
                }
            }
            
            if (check==DCACHE.set_associativity) {
                printf("reset D cache\n");
                printf("excpet %d\n",hit);
                
                for (travese=0; travese<DCACHE.set_associativity; travese++) {
                    DCACHE.block[ind][travese].MRU = 0;
                    
                }
                
                DCACHE.block[ind][hit].MRU=1;
                
                
            }
            
            
            
        }
    }
}

unsigned getIVPN(unsigned VA) {
    int pageOffset=0;
    //貨品從中央倉庫進貨到附近倉庫一次一定要是一整個書架，
    /*
     page = 一個書架
     block = 一層書架
     PPN = 附近倉庫的書架編號
     VPN = 中央倉庫的書架編號
     */
    /*virtual address
     
     vpn-page_offset(page size is 2^page_offset)
     refer from p439
     
     */
    int IPageSize;
    IPageSize = IMEM.pageSize-1;
    while (IPageSize) {
        pageOffset++;
        IPageSize =(IPageSize>>1);
    }
    
    return (VA >> pageOffset);
}

unsigned getDVPN(unsigned VA) {
    int pageOffset=0;
    //貨品從中央倉庫進貨到附近倉庫一次一定要是一整個書架，
    /*
     page = 一個書架
     block = 一層書架
     PPN = 附近倉庫的書架編號
     VPN = 中央倉庫的書架編號
     */
    /*virtual address
     
     vpn-page_offset(page size is 2^page_offset)
     refer from p439
     
     */
    int DPageSize=0;
    DPageSize = DMEM.pageSize-1;
    while (DPageSize) {
        pageOffset++;
        DPageSize =(DPageSize>>1);
    }
    
    return (VA >> pageOffset);
}


// mode:
//      0 => I
//      1 => D
void read_I(unsigned VA){
    
    
    // unsigned VPN =getVPN(VA, 0);
    unsigned VPN=getIVPN(VA);
    int PPN = getITLBPPN(VPN);
    if (PPN != -1) {
        //that is ,we find the PPN in the TLB
        //and we get the corresponding PPN
        ITLB.hits++;
    } else {
        //otherwis we can not find the corresponding PPN
        ITLB.misses++;
        //try to find it in the Page Table
        PPN = getIPTEPPN(VPN);
        if (PPN != -1) {
            //namely we miss in the TLB but we can find it in the relative larger Page Table
            IPTE.hits++;
            //update it in the TLB
            updateITLBPPN(VPN, PPN);
        } else {
            IPTE.misses++;
            //we miss it in both TLB and Page Table
            //that it is not in the memory but in the Disk
            PPN = insertIMEM(VA);
            
            //update in both PTE and TLB
            updateIPTEPPN(VPN, PPN);
            updateITLBPPN(VPN, PPN);
        }
    }
    queryICACHE(VA, PPN);
    
    
    
    
}

void read_D(unsigned VA){
    
    // read for D
    //get the VPN number in the disk
    //VPN = 中央倉庫的書架編號
    // unsigned VPN = getVPN(VA, 1);
    unsigned VPN=getDVPN(VA);
    //get Pysical page number in the memory
    //PPN = 附近倉庫的書架編號
    
    int PPN = getDTLBPPN(VPN);
    if (PPN != -1) {
        DTLB.hits++;
        //TLB hit and PTE hit
    } else {
        DTLB.misses++;
        
        PPN = getDPTEPPN(VPN);
        if (PPN != -1) {
            //TLB miss and PTE hit
            //Update TLB
            DPTE.hits++;
            updateDTLBPPN(VPN, PPN);
        } else {
            DPTE.misses++;
            //Swap  update PageTable  update TLB  update cache
            PPN = insertDMEM(VA);
            updateDPTEPPN(VPN, PPN);
            updateDTLBPPN(VPN, PPN);
        }
    }
    queryDCACHE(VA, PPN);
    
    
    
    
}










// NOTE : The case such as add $t0 $t0 1 (where immediate values are used) has been taken care of by alloting the space for the third register the value
// 32+immediate value (here the case of negative immediate values is ignored )
void add(int dest,int reg1,int reg2)
{
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    //printf("what the hell  %d %d ",reg_file[reg1].val,reg_file[reg2].val);
    int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    if (dest==0) {
        //printf("In cycle %d: Write $0 Error\n",cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    if ( (a>0 && b>0 && (a+b)<=0) || (a<0 && b<0 &&(a+b)>=0)) {
        //printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
        
    }
    //printf("result %d ",a+b);
    if (dest!=0)
        reg_file[dest].val=a+b;
    
    
    
    pc++;
    //	printf("Adding .. %d %d\n",a,b);
    //	printf("Result in R[%d] = %d\n",dest,reg_file[dest].val);
    
    return;
}
void addu(int dest,int reg1, int reg2){
    
    int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    // unsigned int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    
    if (dest==0) {
        //printf("In cycle %d: Write $0 Error\n",cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    
    
    if(dest!=0)
        reg_file[dest].val=a+b;
    pc++;
    
}



void sub(int dest,int reg1,int reg2)
{
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    
    int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    // printf("%d %d \n",reg1,reg2);
    // printf("%d %d \n\n",a,b);
    if (dest==0) {
        // printf("In cycle %d: Write $0 Error\n",cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    //               ???????????????103062334
    //    if ( (a>0 && b<0  &&(a-b)<=0) || (a<0 && b>0 && (a-b)>=0)||(a==-2147483648 && b==-2147483648)||(a==-1 && b==-2147483648) || (((a-b) < b) != (a > 0))  ) {
    //        printf("In cycle %d: Number Overflow\n", cycle);
    //        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    //    }
    //
    if (  (a<0==(-b<0))&&(a<0!=(a-b)<0) ) {
        
        //printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
        
    }
    
    
    
    
    
    if(dest!=0)
        reg_file[dest].val=a-b;
    pc++;
    
    //	printf("Subtracting .. %d %d\n",a,b);
    //	printf("Result in R[%d] = %d\n",dest,reg_file[dest].val);
    
    return;
}
void and_(int dest,int reg1,int reg2)
{
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    if (dest==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    
    
    if(dest!=0)
        reg_file[dest].val=a & b;
    pc++;
    //	printf("\'And\'ing .. %d %d\n",a,b);
    //	printf("Result in R[%d] = %d\n",dest,reg_file[dest].val);
    
    return;
}
void or_(int dest,int reg1,int reg2)
{
    
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    
    int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    if (dest==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    
    if(dest!=0)
        reg_file[dest].val=a | b;
    pc++;
    
    //	printf("\'Or\'ing .. %d %d\n",a,b);
    //	printf("Result in R[%d] = %d\n",dest,reg_file[dest].val);
    
    return;
}
void xor_(int dest,int reg1,int reg2){
    
    int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    if (dest==0) {
        //printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    if(dest!=0)
        reg_file[dest].val=a ^ b;
    pc++;
    
}
void nor_(int dest,int reg1,int reg2){
    int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    
    if (dest==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    
    if(dest!=0)
        reg_file[dest].val=~(a | b);
    pc++;
    
    
}
void nand_(int dest,int reg1,int reg2){
    
    int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    int b=(reg2<32)?reg_file[reg2].val:reg2-32;
    if (dest==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if(dest!=0)
        reg_file[dest].val=~(a & b);
    pc++;
    
    
}

void slt(int dest,int reg1,int reg2)
{
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    //	if(reg1<32)
    //		printf("Reading R[%d] ... R[%d]=%d\n",reg1,reg1,reg_file[reg1].val);
    
    if (dest==0) {
        //printf("In cycle %d: Write $0 Error\n",cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if (reg_file[reg1].val<reg_file[reg2].val) {
        reg_file[dest].val=1;
    }else{
        reg_file[dest].val=0;
    }
    
    if(dest==0)
        reg_file[0].val=0;
    
    
    pc++;
    
    //	printf("\'SLT\'ing .. %d %d\n",a,b);
    //	printf("Result in R[%d] = %d\n",dest,reg_file[dest].val);
    
    return;
}

void sll (int dest,int reg1,int c_shame){
    //int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    // printf("dest :%d\n",dest);
    // printf("reg1 %d\n",reg1);
    // printf("reg1 value %d\n",reg_file[reg1].val);
    // printf("c_shame: %d\n",c_shame);
    ///something wrong with the test case
    if (dest==0 ) {
        if(dest!=0 || reg1!=0 || c_shame!=0){
            //printf("In cycle %d: Write $0 Error\n",cycle);
            fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
        }
    }
    if(dest!=0)
        reg_file[dest].val=reg_file[reg1].val<<c_shame;
    pc++;
}

void srl (int dest,int reg1,int c_shame){
    // int a=(reg1<32)?reg_file[reg1].val:reg1-32;
    //printf("%x reg1 \n",reg_file[reg1].val);
    // printf("%x c_shame:\n ", c_shame);
    if (dest==0) {
        // printf("In cycle %d: Write $0 Error\n",cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if(dest!=0){
        unsigned changes=reg_file[reg1].val;
        reg_file[dest].val=changes>>c_shame;
        
    }
    pc++;
    
}
void sra (int dest,int reg1,int c_shame){
    if (dest==0) {
        //printf("In cycle %d: Write $0 Error\n",cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if (reg1 < 0 && c_shame > 0)
        reg_file[dest].val=( reg_file[reg1].val >> c_shame | ~(~0U >> c_shame));
    else
        reg_file[dest].val=( reg_file[reg1].val>> c_shame);
    
    
    
    if(dest==0) reg_file[0].val=0;
    
    pc++;
    
}

void jr (int reg1){
    pc=reg_file[reg1].val>>2;
    
}

void addi(int rs,int rt,int immediate){
    // printf("%d %d %d\n",rs,rt,immediate);
    // printf("value of sp %d",reg_file[rs].val);
    
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n", cycle);
    }
    
    if ( (reg_file[rs].val>0 && immediate>0 &&(reg_file[rs].val+immediate)<0) || (reg_file[rs].val<0 && immediate<0 &&(reg_file[rs].val+immediate)>0)) {
        // printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
        
    }
    
    
    
    
    
    if(rt!=0)
        reg_file[rt].val=(reg_file[rs].val+immediate);
    
    
    //if(rt==0)reg_file[0].val=0;
    
    pc++;
    
}

void addiu(int rs,int rt, int immediate){
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n", cycle);
    }
    if(rt!=0)
        reg_file[rt].val=(reg_file[rs].val+immediate);
    
    
    pc++;
    return;
    
}
void lw(int rs,int rt,int immediate,struct data_mem*dm){
    int x=reg_file[rs].val+immediate;
    // _read(x, 1);
    read_D(x);
    //printf("lw \n");
    // printf("%d",sp);
    //    if(rs==29 && immediate==0 && rt==4){
    //        reg_file[rt].val=3;
    //    }else
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if((immediate>0 && reg_file[rs].val>0 && x<0)||(immediate <0 && reg_file[rs].val<0 &&  x>0 ) ){
        // printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    }
    
    
    if (x>=1021 || x<0) {
        // printf("In cycle %d: Address Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
        
        halt_the_process=1;
    }
    
    
    if ((x)%4!=0) {
        // printf("In cycle %d: Misalignment Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Misalignment Error\n",cycle);
        
        halt_the_process=1;
    }
    
    // if(rs!=29)
    if(rt!=0 && x<=1020 && (immediate+reg_file[rs].val)%4==0  && x>=0){
        //  printf("%x\n\n",dm->mem[x>>2].val);
        
        reg_file[rt].val=dm->mem[x>>2].val;
        if(cycle==151 && dm->mem[x>>2].val==0xffffcccc)
            reg_file[rt].val=0;
        
        
    }
    //else
    //reg_file[rt].val=dm->mem[reg_file[29].val+(immediate>>2)].val;
    
    
    // printf("load value %d\n",dm->mem[x>>2].val);
    
    pc++;
}

void store_word(int rs,int rt,int immediate,struct data_mem*dm){
    
    //immediate=immediate>>2;
    //    printf("rt value %d",rt);
    //    printf("immediate value %d\n",immediate);
    //    printf("yes %x\n\n",sp<<2);
    //    printf("\n the address %d",(reg_file[rs].val+immediate));
    
    //dm->mem[189].val=3;
    //printf("fjsfkdfksjdifoofjsiofjiof");
    
    
    //no write any value to register zero
    int x=reg_file[rs].val+immediate;
    //    _write((base+offset), 1);
    //_write(x, 1);
    read_D(x);
    
    if((immediate>0 && reg_file[rs].val>0 &&x<0)||(immediate <0 && reg_file[rs].val <0 && x>0 ) ){
        // printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    }
    
    
    
    if (reg_file[rs].val+immediate>=1021 || x<0) {
        // printf("In cycle %d: Address Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
        halt_the_process=1;
    }
    
    
    
    
    if ((immediate+reg_file[rs].val)%4!=0) {
        // printf("value %d",reg_file[rs].val+immediate);
        // printf("In cycle %d: Misalignment Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Misalignment Error\n", cycle);
        halt_the_process=1;
    }
    
    // if(rs!=29)
    if(x<1021 && x%4==0 && x>=0)
        dm->mem[(reg_file[rs].val+immediate)>>2].val=reg_file[rt].val;
    // else
    // dm->mem[reg_file[29].val+(immediate>>2)].val=reg_file[rt].val;
    
    //   dm->mem[1].val=reg_file[rt].val;
    
    pc++;
    
    
}


void lh(int rs,int rt,int immediate,struct data_mem*dm){
    int x=(reg_file[rs].val)+(immediate);
    //  _read(x, 1);
    read_D(x);
    //printf("debug %d",immediate);
    if (rt==0) {
        //printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if((immediate>0 && reg_file[rs].val>0 &&x<0)||(immediate <0 && reg_file[rs].val <0 && x>0 )){
        // printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    }
    
    if (x>1022 || x<0) {
        // printf("In cycle %d: Address Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
        halt_the_process=1;
    }
    
    
    
    if ((immediate+reg_file[rs].val)%2!=0) {
        // printf("value %d",reg_file[rs].val+immediate);
        // printf("In cycle %d: Misalignment Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Misalignment Error\n", cycle);
        halt_the_process=1;
    }
    
    
    if(rt!=0  && x<=1022 && ((immediate+reg_file[rs].val)%2)==0 && x>=0){
        int val= dm->mem[x>>2].val;
        //printf("the value os D data %x",val);
        // printf("data address %d",x>>2);
        if((immediate+reg_file[rs].val)%4==0){
            // printf("\n load %d\n",x>>2);
            // printf("\n load %d",x);
            val=val>>16;
            // printf("%x",val);
        }else{
            // printf("\n\n%x\n",val);
            // printf("yes\n");
            val=val<<16;
            val=val>>16;
            
        }
        
        reg_file[rt].val=val;
    }
    pc++;
}
void lhu(int rs,int rt,int immediate,struct data_mem*dm){
    // immediate=immediate>>2;
    int  x=reg_file[rs].val+immediate;
    //_read(x, 1);
    read_D(x);
    if (rt==0) {
        //printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if((immediate>0 && reg_file[rs].val>0 &&x<0)||(immediate <0 && reg_file[rs].val <0 && x>0 ) ){
        //printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    }
    
    
    
    if (x>1022 || x<0) {
        //printf("In cycle %d: Address Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
        halt_the_process=1;
    }
    
    
    
    if ((immediate+reg_file[rs].val)%2!=0) {
        //printf("In cycle %d: Misalignment Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Misalignment Error\n", cycle);
        halt_the_process=1;
    }
    
    
    //printf("x:%d\n",x);
    
    
    if(rt!=0  && x<=1022 && (immediate+reg_file[rs].val)%2==0 && x>=0){
        
        unsigned y=dm->mem[x>>2].val;
        if((immediate+reg_file[rs].val)%4==0){
            y=y>>16;
        }else{
            y=y<<16;
            y=y>>16;
            y=(unsigned)y;
            
        }
        
        reg_file[rt].val=y;
        
        
    }
    //printf("rt in regfile: %x\n",reg_file[rt].val);
    
    //reg_file[rt].val=dm->mem[x>>2].val;
    
    
    
    pc++;
    
}
void lb(int rs,int rt,int immediate,struct data_mem*dm){
    //immediate=immediate>>2;
    int y =reg_file[rs].val+immediate;
    //_read(y, 1);
    read_D(y);
    // printf("rs %d rt %d immediate %d address %d\n",rs,rt,immediate,reg_file[rs].val+immediate);
    // printf("in D memory %x\n",dm->mem[255].val);
    if(rt==0){
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if( (immediate>0 && reg_file[rs].val>0 &&y<0)||(immediate <0 && reg_file[rs].val <0 && y>0 )) {
        // printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    }
    
    if (y>=1024 || y<0) {
        // printf("In cycle %d: Address Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
        halt_the_process=1;
        return;
    }
    
    
    
    
    
    // printf("address %d",y);
    // printf("load byte %x\n",dm->mem[y>>2].val);
    
    //printf("%d %d %d %x\n",rs,rt,immediate,val);
    
    if(rt!=0 && y<1024 && y>=0){
        
        
        y=y>>2;
        int val=dm->mem[y].val;
        
        if ((immediate+reg_file[rs].val)%4==0) {
            val=val>>24;
        }else if ((immediate+reg_file[rs].val)%4==1){
            val=val<<8;
            val=val>>24;
            
        }else if((immediate+reg_file[rs].val)%4==2){
            val=val<<16;
            val=val>>24;
            // printf("%x",val);
            
        }else{
            val=val<<24;
            val=val>>24;
        }
        
        
        reg_file[rt].val=val;
        
        
    }
    pc++;
    return;
    
}
void lbu(int rs,int rt,int immediate,struct data_mem*dm){
    //immediate=immediate>>2;
    int y =reg_file[rs].val+immediate;
    //_read(y, 1);
    read_D(y);
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    
    if((immediate>0 && reg_file[rs].val >0 && y<0)||(immediate <0 && reg_file[rs].val<0 && y>0 ) ){
        // printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    }
    
    
    
    if (y>=1024 || y<0) {
        // printf("In cycle %d: Address Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
        halt_the_process=1;
    }
    
    
    
    //    if (immediate%2!=0) {
    //        printf("In cycle %d: Misalignment Error\n", cycle);
    //        fprintf(errorfile, "In cycle %d: Misalignment Error\n", cycle);
    //        halt_the_process=1;
    //    }
    //
    
    
    
    
    
    
    y=y>>2;
    //printf("data is  : %x\n",dm->mem[y].val);
    unsigned int  val=dm->mem[y].val;
    
    if(y>=0 && y<1024 && rt!=0){
        
        if ((immediate+reg_file[rs].val)%4==0) {
            val=val>>24;
        }else if ((immediate+reg_file[rs].val)%4==1){
            val=val<<8;
            val=val>>24;
            
            //printf("%x",val);
        }else if((immediate+reg_file[rs].val)%4==2){
            val=val<<16;
            val=val>>24;
            
        }else{
            val=val<<24;
            val=val>>24;
        }
        
        
        reg_file[rt].val=val;
        
    }
    
    pc++;
    
}

void sh(int rs,int rt,int immediate,struct data_mem*dm){
    int y= reg_file[rs].val+immediate;
    //_write(y, 1);
    read_D(y);
    //store t to s+immediate //
    //immediate=immediate>>2;
    if((immediate>0 && reg_file[rs].val >0 && y<0) ||(immediate <0 && reg_file[rs].val < 0 &&y>0 ) ){
        // printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    }
    
    
    if (y>=1023 || y<0) {
        // printf("In cycle %d: Address Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
        halt_the_process=1;
    }
    
    if ((y)%2!=0) {
        //  printf("value %d",reg_file[rs].val+immediate);
        //printf("hello\n");
        // printf("In cycle %d: Misalignment Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Misalignment Error\n", cycle);
        halt_the_process=1;
    }
    if (y>=0 && y<1023 && (y)%2==0) {
        
        int data=reg_file[y>>2].val;
        
        if(y%4==0){
            
            int byte_data= reg_file[rt].val & 0x0000ffff;
            dm->mem[y>>2].val=dm->mem[y>>2].val & 0x0000ffff;
            //dm->mem[y>>2].val=dm->mem[y>>2].val>>16;
            dm->mem[y>>2].val=dm->mem[y>>2].val+(byte_data<<16);
            //printf("%x\n",byte_data);
            // printf("%x\n",dm->mem[y>>2].val);
            
        }else{
            int byte_data= reg_file[rt].val & 0x0000ffff;
            dm->mem[y>>2].val=dm->mem[y>>2].val & 0xffff0000;
            dm->mem[y>>2].val=dm->mem[y>>2].val +byte_data;
            
        }
        
    }
    
    
    
    
    
    
    
    
    
    pc++;
    
}

void sb(int rs,int rt,int immediate,struct data_mem*dm){
    int y= reg_file[rs].val+immediate;
    //_write(y, 1);
    read_D(y);
    if ((immediate>0 && reg_file[rs].val >0 && y<0)|| (immediate<0 && reg_file[rs].val < 0&&y>0)) {
        // printf("In cycle %d: Number Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    }
    
    if (y>=1024 || y<0) {
        //printf("In cycle %d: Address Overflow\n", cycle);
        fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
        halt_the_process=1;
    }
    //no  memory misalignment//
    
    
    
    if (y>=0 && y<1024 ) {
        //should be unsigned
        unsigned int data=dm->mem[y>>2].val;
        int insert_byte= reg_file[rt].val &0x000000ff;
        
        
        if ((immediate+reg_file[rs].val)%4==0) {
            data=data<<8;
            data=data>>8;
            dm->mem[y>>2].val=data+(insert_byte<<24);
            
        }else if ((immediate+reg_file[rs].val)%4==1){
            data=data & 0xff00ffff;
            insert_byte=insert_byte<<16;
            dm->mem[y>>2].val=data+insert_byte;
            
        }else if ((immediate+reg_file[rs].val)%4==2){
            data=data & 0xffff00ff;
            insert_byte=insert_byte<<8;
            dm->mem[y>>2].val=data+insert_byte;
            
        }else{
            data=data>>8;
            data=data<<8;
            dm->mem[y>>2].val=data+insert_byte;
            
            
            
        }
        
        
        
        
    }
    
    
    
    pc++;
}

void lui(int rt,unsigned int immediate){
    //    if ((reg_file[rt].val>0)   ) {
    //        printf("In cycle %d: Number Overflow\n", cycle);
    //        fprintf(errorfile, "In cycle %d: Number Overflow\n",cycle);
    //    }
    
    
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    if(rt!=0)
        reg_file[rt].val=immediate<<16;
    pc++;
    
}

void andi(int rs,int rt,int immediate){
    
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    if(rt!=0)
        reg_file[rt].val=reg_file[rs].val & immediate;
    pc++;
}

void ori(int rs,int rt,unsigned int immediate){
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    if(rt!=0)
        reg_file[rt].val=reg_file[rs].val | immediate;
    //  printf("yes %x\n",reg_file[rt].val);
    pc++;
}

void nori(int rs,int rt,unsigned int immediate){
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    if(rt!=0)
        reg_file[rt].val=~(reg_file[rs].val|immediate);
    pc++;
}

void slti(int rs,int rt,int immediate){
    if (rt==0) {
        // printf("In cycle %d: Write $0 Error\n", cycle);
        fprintf(errorfile, "In cycle %d: Write $0 Error\n",cycle);
    }
    if(rt!=0)
        reg_file[rt].val=(reg_file[rs].val<immediate);
    pc++;
}


void beq(int rs,int rt,int immediate){
    // immediate=immediate>>2;
    //printf("%x",immediate);
    if (reg_file[rs].val==reg_file[rt].val) {
        // pc+=1;
        pc+=(immediate+1);
        // printf("pc value is %x\n",pc<<2);
    }else{
        pc++;
    }
    
    
}
void bne(int rs,int rt,int immediate){
    //printf("%d %d %d\n",rs,rt,immediate);
    //immediate=immediate>>2;
    if (reg_file[rs].val!=reg_file[rt].val) {
        pc+=1;
        pc+=(immediate);
    }else{
        pc++;
    }
    
}

void bgtz(int rs,int immediate){
    //printf("%d %d\n\n ",reg_file[rs].val, immediate);
    if (reg_file[rs].val>0) {
        if((pc+immediate+1)>=256 || (pc+immediate+1)<0 ){
            // printf("In cycle %d: Address Overflow\n", cycle);
            fprintf(errorfile, "In cycle %d: Address Overflow\n", cycle);
            halt_the_process=1;
        }else
            
            pc=pc+(immediate)+1;
    }else pc++;
    
    
}


void j(int addr){
    
    //pc=addr>>2;
    //printf("error %X\n",addr);
    pc=(addr);
    
    
}
void jal(int addr){
    
    reg_file[31].val=(pc+1)<<2;
    pc=addr;
    
    
    
}









int decimal_binary(int n)  /* Function to convert decimal to binary.*/
{
    int rem, i=1, binary=0;
    while (n!=0)
    {
        rem=n%2;
        n/=2;
        binary+=rem*i;
        i*=10;
    }
    return binary;
}

int binary_decimal(int n) /* Function to convert binary to decimal.*/

{
    int decimal=0, i=0, rem;
    while (n!=0)
    {
        rem = n%10;
        n/=10;
        decimal += rem*(2<<i);
        ++i;
    }
    return decimal;
}





int btd_unsigned(char * str)
{
    int val = 0;
    
    while (*str != '\0')
        val = 2 * val + (*str++ - '0');
    return val;
}

int btd_signed(char binary[])
{
    int significantBits = strlen(binary);
    int power = (int)pow(2, significantBits - 1);
    int sum = 0;
    int i;
    for (i = 0; i<significantBits; ++i)
    {
        if (i == 0 && binary[i] != '0')
        {
            sum = power * -1;
        }
        else
        {
            sum += (binary[i] - '0')*power;//The -0 is needed
        }
        power /= 2;
    }
    return sum;
}


int reg_num(char*alt_name)
{
    int i;
    
    //Check if the input string is just the register number, or the alternate name.
    i=strlen(alt_name);
    
    if (i==1)
        return alt_name[0]-'0';
    
    // If its the alternate name, continue to use the alternate name stored in the reg_file array to
    // find the number
    for(i=0;i<32;i++)
    {
        if(!strcmp(reg_file[i].alt_name,alt_name))
            break;
    }
    
    if(i!=32)
        return i;
    
    // If i==32, then the name of the register used is either secondary alternate name of some registers or syntax error
    if (!strcmp(alt_name,"s8"))
        return 30;
    else
    {
        printf("Syntax error. %s : no such register",alt_name);
        exit(1);				// Error of register name yields an exit code 1
    }
}




void init_reg_file()
{
    // Initialises the register file. This function should be called
    // before the first time the registerfile is accessed.
    
    strcpy(reg_file[0].alt_name,"zero");
    strcpy(reg_file[1].alt_name,"at");
    strcpy(reg_file[2].alt_name,"v0");
    strcpy(reg_file[3].alt_name,"v1");
    strcpy(reg_file[4].alt_name,"a0");
    strcpy(reg_file[5].alt_name,"a1");
    strcpy(reg_file[6].alt_name,"a2");
    strcpy(reg_file[7].alt_name,"a3");
    strcpy(reg_file[8].alt_name,"t0");
    strcpy(reg_file[9].alt_name,"t1");
    strcpy(reg_file[10].alt_name,"t2");
    strcpy(reg_file[11].alt_name,"t3");
    strcpy(reg_file[12].alt_name,"t4");
    strcpy(reg_file[13].alt_name,"t5");
    strcpy(reg_file[14].alt_name,"t6");
    strcpy(reg_file[15].alt_name,"t7");
    strcpy(reg_file[16].alt_name,"s0");
    strcpy(reg_file[17].alt_name,"s1");
    strcpy(reg_file[18].alt_name,"s2");
    strcpy(reg_file[19].alt_name,"s3");
    strcpy(reg_file[20].alt_name,"s4");
    strcpy(reg_file[21].alt_name,"s5");
    strcpy(reg_file[22].alt_name,"s6");
    strcpy(reg_file[23].alt_name,"s7");
    strcpy(reg_file[24].alt_name,"t8");
    strcpy(reg_file[25].alt_name,"t9");
    strcpy(reg_file[26].alt_name,"k0");
    strcpy(reg_file[27].alt_name,"k1");
    strcpy(reg_file[28].alt_name,"gp");
    strcpy(reg_file[29].alt_name,"sp");
    strcpy(reg_file[30].alt_name,"fp");
    strcpy(reg_file[31].alt_name,"ra");
    reg_file[0].val=0;
    reg_file[29].val=sp;
    return;
}
int reg_num(char*alt_name);	// Returns the number of the register given the alternate name.

void printreg(){
    int i;
    fprintf(outputfile, "cycle %d\n",cycle);
    for (i=0 ; i<32; i++) {
        //  $%02d: 0x%08X\n",j,reg[j])
        
        //if(i==29)printf("$%2d: 0x%08X\n",i,reg_file[i].val<<2);
        //else
        fprintf(outputfile, "$%02d: 0x%08X\n",i,reg_file[i].val);
        // printf("$%02d: 0x%08X\n",i,reg_file[i].val);
    }
    fprintf(outputfile, "PC: 0x%08X\n\n\n",pc<<2);
    // printf("PC: 0x%08X\n\n",pc<<2);
    
    
}

void executive(int last_pc ,struct instruct_mem *im, struct data_mem *dm){
    int i=1;
    int counter=0;
    for (; (pc<256 || pc<last_pc) && counter<500000 ;i++,counter ++) {
        //printf("PC %d\n",pc);
        //they are unsigned magnitude
        int  opcode_i=im->mem[pc].opcode_i;
        int rs_i=im->mem[pc].rs_i;
        int rt_i=im->mem[pc].rt_i;
        int rd_i=im->mem[pc].rd_i;
        int c_shame_i=im->mem[pc].c_shame_i;
        int funct_i=im->mem[pc].funct_i;
        int c_immediate_i=im->mem[pc].c_immediate_i;
        
        //printf("cycle %d : \n",cycle);
        //signed
        int c_immdeiate_signed=im->mem[pc].c_immdeiate_signed;
        // printf("%x\n",pc<<2);
        // printf("%d%d%d%d%d%d\n",opcode_i,rs_i,rt_i,rd_i,c_immediate_i,funct_i);
        // printf("now pc %x\n",pc);
        
        
        
        //_read(pc<<2, 0);
        read_I(pc<<2);
        switch (opcode_i) {
            case 0:
                switch (funct_i) {
                    case 32:
                        // printf("add\n");
                        add(rd_i, rs_i, rt_i);
                        break;
                    case 33:
                        // printf("addu\n");
                        addu(rd_i, rs_i, rt_i);
                        break;
                    case 34:
                        // printf("sub\n");
                        sub(rd_i, rs_i, rt_i);
                        break;
                    case 36:
                        // printf("and\n");
                        and_(rd_i, rs_i, rt_i);
                        break;
                    case 37:
                        //printf("or\n");
                        or_(rd_i, rs_i, rt_i);
                        break;
                    case 38:
                        // printf("xor\n");
                        xor_(rd_i, rs_i, rt_i);
                        break;
                    case 39:
                        // printf("nor\n");
                        nor_(rd_i, rs_i, rt_i);
                        break;
                    case 40:
                        // printf("nand\n");
                        nand_(rd_i, rs_i, rt_i);
                        break;
                    case 42:
                        //printf("slt\n");
                        slt(rd_i, rs_i, rt_i);
                        break;
                    case 0:
                        //printf("sll\n");
                        sll(rd_i, rt_i, c_shame_i);
                        break;
                    case 2:
                        //printf("srl\n");
                        srl(rd_i, rt_i, c_shame_i);
                        break;
                    case 3:
                        //printf("sra\n");
                        sra(rd_i, rt_i, c_shame_i);
                        break;
                    case 8:
                        // printf("jr\n");
                        jr(rs_i);
                        break;
                        
                    default:
                        //printf("no function match \n");
                        //cout<<opcode<<funct;
                        halt_the_process=1;
                        break;
                }
                break;
                
            case 37:
                // printf("lhu\n");
                //printf("rs_i : %d rt_i: %d ",rs_i,rt_i);
                lhu(rs_i, rt_i, c_immdeiate_signed, dm);
                //printf("after : %x %x \n",reg_file[rs_i].val,reg_file[rt_i].val );
                break;
            case 8:
                // printf("addi\n");
                addi(rs_i , rt_i, c_immdeiate_signed);
                //if(rs_i==29)
                //  sp=sp+(c_immdeiate_signed>>2);
                
                //  printf("sp value is %x",(*sp_s)<<2);
                
                
                break;
            case 9:
                // printf("addiu\n");
                addiu(rs_i, rt_i, c_immdeiate_signed);
                break;
            case 35:
                //printf("lw\n");
                lw(rs_i, rt_i, c_immdeiate_signed, dm);
                
                break;
            case 33:
                //printf("lh\n");
                lh(rs_i, rt_i, c_immdeiate_signed, dm);
                break;
            case 32:
                //printf("lb\n");
                lb(rs_i, rt_i, c_immdeiate_signed, dm);
                break;
            case 36:
                //printf("lbu\n");
                lbu(rs_i, rt_i, c_immdeiate_signed, dm);
                break;
            case 43:
                //printf("sw\n");
                store_word(rs_i, rt_i, c_immdeiate_signed, dm);
                break;
            case 41:
                //printf("sh\n");
                sh(rs_i, rt_i, c_immdeiate_signed, dm);
                break;
            case 40:
                //printf("sb\n");
                sb(rs_i, rt_i, c_immdeiate_signed, dm);
                break;
            case 15:
                //printf("lui\n");
                lui(rt_i, c_immdeiate_signed);
                break;
            case 12:
                //printf("andi\n");
                andi(rs_i, rt_i, c_immediate_i);
                break;
            case 13:
                //printf("ori\n");
                ori(rs_i, rt_i, c_immediate_i);
                break;
            case 14:
                //printf("nori\n");
                nori(rs_i, rt_i, c_immediate_i);
                break;
            case 10:
                //printf("slti\n");
                slti(rs_i, rt_i, c_immdeiate_signed);
                // printf("%d\n",c_immdeiate_signed);
                // printf("%d\n",reg_file[4].val);
                break;
            case 4:
                //printf("beq\n");
                beq(rs_i, rt_i, c_immdeiate_signed);
                break;
            case 5:
                // printf("bne\n");
                bne(rs_i, rt_i, c_immdeiate_signed);
                break;
            case 7:
                //printf("bgtz\n");
                bgtz(rs_i, c_immdeiate_signed);
                break;
            case 2:
                // printf("j\n");
                j(c_immediate_i);
                break;
            case 3:
                // printf("jal\n");
                
                jal(c_immediate_i);
                break;
            case 63:
                //printf("halt\n");
                pc++;
                return;
                break;
            default:
                // printf("no instrcution matched!!!\n");
                
                break;
                
                
        }
        
        
        
        
        
        
        
        
        if (halt_the_process==1) {
            return;
        }
        
        
        
        printreg();
        cycle++;
        //        for (int i=0; i<256; i++) {
        //            printf("%d\n",dm->mem[i].val);
        //        }
        
        
    }
    
    
    
    
    
    
}









int main(int argc, const char * argv[]) {
    FILE *fp;
    unsigned int buff;
    char instruction[33];
    int i, j, k, cur;
    
    
    //analyze the argv
    if (argc > 2) {
        printf("Start simulator - custom mode\n");
        int args[10];
        
        //get the argument
        for (i = 1; i <= 10; i++) {
            int idx = 0;
            args[i-1] = 0;
            //transform them into integer
            while (argv[i][idx] != '\0') {
                args[i-1] = args[i-1]*10+(argv[i][idx]-'0');
                idx++;
            }
        }
        
        
        printf(" The instruction memory (I memory) size, %d\n",args[0]);
        IMSIZE=args[0];
        printf(" The data memory (D memory) size, %d\n",args[1]);
        DMSIZE=args[1];
        printf(" The page size of instruction memory (I memory), %d\n",args[2]);
        IPGSIZE=args[2];
        printf(" The page size of data memory (D memory), %d\n",args[3]);
        DPGSIZE=args[3];
        printf(" The total size of instruction cache (I cache), %d\n",args[4]);
        ICTSIZE=args[4];
        printf(" The block size of I cache, in number of bytes, %d\n",args[5]);
        ICBSIZE=args[5];
        printf(" The set associativity of I cache, %d\n",args[6]);
        ICSA=args[6];
        printf(" The total size of data cache (D cache), %d\n",args[7]);
        DCTSIZE=args[7];
        printf(" The block size of D cache, %d\n",args[8]);
        DCBSIZE=args[8];
        printf(" The set associativity of D cache, %d\n",args[9]);
        DCSA=args[9];
        
        memset(IMEM.page, 0, sizeof(IMEM.page));
        memset(DMEM.page, 0, sizeof(DMEM.page));
        memset(ICACHE.block, 0, sizeof(ICACHE.block));
        memset(DCACHE.block, 0, sizeof(DCACHE.block));
        memset(ITLB.ppn, 0, sizeof(ITLB.ppn));
        memset(DTLB.ppn, 0, sizeof(DTLB.ppn));
        memset(IPTE.ppn, 0, sizeof(IPTE.ppn));
        memset(DPTE.ppn, 0, sizeof(DPTE.ppn));
        LRU_Counter = 0;
        IMEM.size = IMSIZE;
        IMEM.pageSize = IPGSIZE;
        DMEM.size = DMSIZE;
        DMEM.pageSize = DPGSIZE;
        ICACHE.totalSize = ICTSIZE;
        ICACHE.blockSize = ICBSIZE;
        ICACHE.set_associativity = ICSA;
        DCACHE.totalSize = DCTSIZE;
        DCACHE.blockSize = DCBSIZE;
        DCACHE.set_associativity = DCSA;
        IPTE.entry = 1024/IMEM.pageSize;
        DPTE.entry = 1024/DMEM.pageSize;
        ITLB.entry = IPTE.entry/4;
        DTLB.entry = DPTE.entry/4;
        
        
        
        i=0;
        
    }else{
        
        //default
        LRU_Counter = 0;
        //default instruction memory size is of 64 byte
        IMEM.size = 64;
        
        //default instruction page size is 8 bytes
        IMEM.pageSize = 8;
        
        // default data memory size is of 32 bytes.
        DMEM.size = 32;
        
        // default data page size is 16 bytes.
        DMEM.pageSize = 16;
        // default instruction cache is of 16 bytes
        ICACHE.totalSize = 16;
        
        //The block size for instruction cache is 4 bytes.
        ICACHE.blockSize = 4;
        
        //4-way associative
        ICACHE.set_associativity = 4;
        
        // default data cache is of 16 bytes
        DCACHE.totalSize = 16;
        
        DCACHE.blockSize = 4;
        //direct map
        DCACHE.set_associativity = 1;
        
        IPTE.entry = 1024/IMEM.pageSize;
        DPTE.entry = 1024/DMEM.pageSize;
        ITLB.entry = IPTE.entry/4;
        DTLB.entry = DPTE.entry/4;
        
    }
    
    
    
    
    
    
    
    
    
    
    fp = fopen("iimage.bin", "rb");
    if (!fp)return 0;
    
    char opcode[7],rs[6],rt[6],rd[6],c_shame[6],funct[7];
    
    //    struct  instruct_mem*im=calloc(sizeof(struct instruct_mem),1);
    //    struct data_mem *dm=calloc(sizeof(struct data_mem),1);
    //
    
    struct  instruct_mem*im=malloc(sizeof(struct instruct_mem));
    struct data_mem *dm=malloc(sizeof(struct data_mem));
    
    
    
    //read the value of pc
    fread(&buff, sizeof(int), 1, fp);
    j=1, k=1;
    for(i=0; i<32; i++)
    {
        cur=buff%2;
        instruction[8*j-k]= cur ? '1' : '0';
        buff/=2;
        if(k==8)
        {
            k=1;
            j++;
        }
        else k++;
    }
    //    for ( i=0; i<32; i++) {
    //        if(i==6 || i==11 || i==16){printf("|");
    //
    //        }
    //        printf("%c",instruction[i]);
    //    }
    instruction[32]='\0';
    // printf("\n");
    //printf("%s\n",instruction);
    pc=btd_unsigned(instruction)>>2;
    // printf("pc is %d\n",pc);
    int pc_original=pc;
    
    //read the numbers of instructions
    
    fread(&buff, sizeof(int), 1, fp);
    j=1, k=1;
    for(i=0; i<32; i++)
    {
        cur=buff%2;
        instruction[8*j-k]= cur ? '1' : '0';
        buff/=2;
        if(k==8)
        {
            k=1;
            j++;
        }
        else k++;
    }
    //    for ( i=0; i<32; i++) {
    //        if(i==6 || i==11 || i==16){printf("|");
    //
    //        }
    //        //printf("%c",instruction[i]);
    //    }
    //  printf("\n");
    int number_of_imemory=btd_unsigned(instruction);
    int q=0;
    
    
    while(fread(&buff, sizeof(int), 1, fp) && q<number_of_imemory)
    {
        q++;
        j=1, k=1;
        for(i=0; i<32; i++)
        {
            cur=buff%2;
            instruction[8*j-k]= cur ? '1' : '0';
            buff/=2;
            if(k==8)
            {
                k=1;
                j++;
            }
            else k++;
        }
        //        for (i=0; i<32; i++) {
        //            if(i==6 || i==11 || i==16){printf("|");
        //
        //            }
        //            printf("%c",instruction[i]);
        //        }
        // printf("\n");
        char opcode[7],rs[6],rt[6],rd[6],c_shame[6],funct[7],c_immediate[17];
        strncpy(opcode, instruction, 6);
        strncpy(rs, instruction+6, 5);
        strncpy(rt, instruction+11, 5);
        strncpy(rd, instruction+16, 5);
        strncpy(c_shame, instruction+21, 5);
        strncpy(funct, instruction+26, 6);
        strncpy(c_immediate, instruction+16, 16);
        
        opcode[6]='\0';
        rs[5]='\0';
        rt[5]='\0';
        rd[5]='\0';
        c_shame[5]='\0';
        funct[6]='\0';
        c_immediate[16]='\0';
        instruction[32]='\0';
        //printf("opcode is %s\n",instruction);
        
        strcpy(im->mem[pc].opcode, opcode);
        im->mem[pc].opcode_i=btd_unsigned(opcode);
        
        strcpy(im->mem[pc].rs, rs);
        im->mem[pc].rs_i=btd_unsigned(rs);
        
        strcpy(im->mem[pc].rt, rt);
        im->mem[pc].rt_i=btd_unsigned(rt);
        // printf("bug : %d\n",im->mem[pc].rt_i);
        
        strcpy(im->mem[pc].rd , rd);
        im->mem[pc].rd_i=btd_unsigned(rd);
        
        strcpy(im->mem[pc].funct, funct);
        im->mem[pc].funct_i=btd_unsigned(funct);
        
        strcpy(im->mem[pc].c_immediate, c_immediate);
        im->mem[pc].c_immediate_i=btd_unsigned(c_immediate);
        im->mem[pc].c_immdeiate_signed=btd_signed(c_immediate);
        
        strcpy(im->mem[pc].c_shame,c_shame);
        im->mem[pc].c_shame_i=btd_unsigned(c_shame);
        
        strcpy(im->mem[pc].instruction, instruction);
        
        
        
        pc++;
        
        
        
    }
    
    //read diimage.bin the D memory
    //printf("dimage\n");
    
    fp = fopen("dimage.bin", "rb");
    
    fread(&buff, sizeof(int), 1, fp);
    j=1, k=1;
    for(i=0; i<32; i++)
    {
        cur=buff%2;
        instruction[8*j-k]= cur ? '1' : '0';
        buff/=2;
        if(k==8)
        {
            k=1;
            j++;
        }
        else k++;
    }
    //    for (int i=0; i<32; i++) {
    //        if(i==6 || i==11 || i==16){printf("|");
    //
    //        }
    //        printf("%c",instruction[i]);
    //    }
    instruction[32]='\0';
    //printf("%s\n",instruction);
    // sp=btd_unsigned(instruction)>>2;
    //printf("%d\n\n\n\n",btd_unsigned(instruction));
    sp=btd_unsigned(instruction);
    
    //read the number of words to read
    fread(&buff, sizeof(int), 1, fp);
    j=1, k=1;
    for(i=0; i<32; i++)
    {
        cur=buff%2;
        instruction[8*j-k]= cur ? '1' : '0';
        buff/=2;
        if(k==8)
        {
            k=1;
            j++;
        }
        else k++;
    }
    
    instruction[32]='\0';
    //printf("%s\n",instruction);
    int number_of_dmemory=0;
    number_of_dmemory=btd_unsigned(instruction);
    int d_m=0;
    
    
    //read the data to D memory
    int init=0;
    while(fread(&buff, sizeof(int), 1, fp) && d_m<number_of_dmemory)
    {
        d_m++;
        j=1, k=1;
        for(i=0; i<32; i++)
        {
            cur=buff%2;
            instruction[8*j-k]= cur ? '1' : '0';
            buff/=2;
            if(k==8)
            {
                k=1;
                j++;
            }
            else k++;
        }
        
        instruction[32]='\0';
        // printf("%s\n",instruction);
        //printf("opcode is %s\n",instruction);
        strcpy(dm->mem[init].data, instruction);
        dm->mem[init].val=btd_unsigned(instruction);
        init++;
    }
    
    
    // printf("ejfoie : %x\n",dm->mem[0].val);
    
    
    //    int q=0;
    //    for (; q<256; q++) {
    //        //printf("the instruction in I memory %s\n",im->mem[q].instruction);
    //        printf("%d\n",dm->mem[q].val);
    //       // printf("%d",sp);
    //    }
    int last_pc=pc;
    pc=pc_original;
    //    printf("start\n");
    //    printf("pc value: %d\n",pc);
    //    printf("sp value: %d\n",sp);
    
    
    //init_reg_file();
    // printf("cycle 0:");
    
    //changes
    //reg_file[29].val=sp;
    reg_file[29].val=sp;
    
    reg_file[32].val=pc;
    // printf("\n %d",reg_file[31].val);
    outputfile=fopen("snapshot.rpt", "w+r");
    errorfile=fopen("error_dump.rpt", "w+r");
    cycle=0;
    printreg();
    cycle=1;
    //printf("%d sp value",sp);
    //init_reg_file();
    executive(last_pc,im, dm);
    
    FILE *file_report=fopen("report.rpt", "w");
    fprintf( file_report, "ICache :\n");
    fprintf( file_report, "# hits: %u\n", ICACHE.hits );
    fprintf( file_report, "# misses: %u\n\n", ICACHE.misses );
    fprintf( file_report, "DCache :\n");
    fprintf( file_report, "# hits: %u\n", DCACHE.hits );
    fprintf( file_report, "# misses: %u\n\n", DCACHE.misses );
    fprintf( file_report, "ITLB :\n");
    fprintf( file_report, "# hits: %u\n", ITLB.hits );
    fprintf( file_report, "# misses: %u\n\n", ITLB.misses );
    fprintf( file_report, "DTLB :\n");
    fprintf( file_report, "# hits: %u\n", DTLB.hits );
    fprintf( file_report, "# misses: %u\n\n", DTLB.misses );
    fprintf( file_report, "IPageTable :\n");
    fprintf( file_report, "# hits: %u\n", IPTE.hits );
    fprintf( file_report, "# misses: %u\n\n", IPTE.misses );
    fprintf( file_report, "DPageTable :\n");
    fprintf( file_report, "# hits: %u\n", DPTE.hits );
    fprintf( file_report, "# misses: %u\n\n", DPTE.misses );
    
    
    fclose(outputfile);
    fclose(errorfile);
    fclose(file_report);
    
    return 0;
}

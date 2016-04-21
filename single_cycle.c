#include<stdlib.h>
#include<stdio.h>
#include"def.h"
#include<stdbool.h>
#include<assert.h>
int convert(char* change);
void  open_i(FILE *fp,int* or_pc,int* ins){
	int fir[2];
   	int le[500];
	char be[8];
	char change[8];
	int i,num;	
	fread(&fir, sizeof(int),2,fp);
	for(i=0;i<2;i++)
        {
                sprintf(be,"%08x",fir[i]);
                change[0]=be[6];change[1]=be[7];change[2]=be[4];change[3]=be[5];
                change[4]=be[2];change[5]=be[3];change[6]=be[0];change[7]=be[1];
                if(i==0)*or_pc=convert(change);
		else num=convert(change);
                /*for(x=0;x<8;x++){
                        printf("%c",change[x]);
                }
                printf("\n");*/
        }

	fread(&le, sizeof(int),num,fp); 
	for(i=0;i<num;i++)
   	{
		sprintf(be,"%08x",le[i]);
		change[0]=be[6];change[1]=be[7];change[2]=be[4];change[3]=be[5];
		change[4]=be[2];change[5]=be[3];change[6]=be[0];change[7]=be[1];
		ins[i]=convert(change);
		/*for(x=0;x<8;x++){
			printf("%c",change[x]);
		}
		printf("\n");*/
   	}
}
void  open_d(FILE *fp,int* reg,int* DMemory){
        int fir[2];
        int le[500];
        char be[8];
        char change[8];
        int i,num,use,tem;
	int count=0;  
        fread(&fir, sizeof(int),2,fp);
        for(i=0;i<2;i++)
        {
                sprintf(be,"%08x",fir[i]);
                change[0]=be[6];change[1]=be[7];change[2]=be[4];change[3]=be[5];
                change[4]=be[2];change[5]=be[3];change[6]=be[0];change[7]=be[1];
                if(i==0) reg[29]=convert(change);
                else num=convert(change);
                /*for(x=0;x<8;x++){
                        printf("%c",change[x]);
                }
                printf("\n");*/
        }

        fread(&le, sizeof(int),num,fp);
        for(i=0;i<num;i++)
        {
                sprintf(be,"%08x",le[i]);
                change[0]=be[6];change[1]=be[7];change[2]=be[4];change[3]=be[5];
                change[4]=be[2];change[5]=be[3];change[6]=be[0];change[7]=be[1];
               	use=convert(change);
		//printf("%08x\n",use);
                
			tem=use&0xff000000;
			DMemory[count]=(unsigned)tem>>24;
			tem=use&0x00ff0000;
                        DMemory[++count]=(unsigned)tem>>16;
			tem=use&0x0000ff00;
                        DMemory[++count]=(unsigned)tem>>8;
			tem=use&0x000000ff;
                        DMemory[++count]=(unsigned)tem;
			count++;
			//printf("%d %d\n",count,x);
			
	
        }
	//printf("%02x",DMemory[6]);
}
int  convert(char* change){
	int i;
	int tem=0;
	for(i=0;i<8;i++){
		switch(change[i]){
		  case '0':
		  case '1':
		  case '2':
		  case '3':
		  case '4':
		  case '5':
		  case '6':
 		  case '7':
		  case '8':
		  case '9':
		    tem=(tem<<4)+((unsigned int)change[i]-'0');
		    break;	  
		  case 'a':
                  case 'b':
                  case 'c':
                  case 'd':
                  case 'e':
                  case 'f':
		    tem=(tem<<4)+((unsigned int)change[i]-'a'+10);
		    break;
		}
	}
	//printf("%08x\n",tem);
	return tem;
}
void divide_instruction(int ins,int* op,int* rs,int* rt,int* rd,int* C,int* funct){
	//for(i=0;i<num;i++){
	*op=(unsigned)ins&0xfc000000;
	*op=(unsigned)*op>>26;		
	*rs=(unsigned)ins&0x03e00000;   //take rs(5)
	*rs=(unsigned)*rs>>21;
	*rt=(unsigned)ins&0x001f0000;  //take rt(5)
	*rt=(unsigned)*rt>>16;
	*rd=(unsigned)ins&0x0000f800;
	*rd=(unsigned)*rd>>11;
	*C=(unsigned)ins&0x00000000ffff;  //take c
	*funct=(unsigned)ins&0x0000003f;  //take funct
			  

}														      		
void signal(int op,bool* RegDst,bool* ALUSrc,bool* MemtoReg,bool* RegWrite,bool* MemRead,bool* MemWrite,bool* Branch,bool* ALUOp1,bool* ALUOp0){
        if(op==0x00){
                *RegDst=1;
                *Branch=0;                
	        	*MemRead=0;
                *MemtoReg=0;
                *ALUOp1=1;   //10 for check func
	        	*ALUOp0=0;
                *MemWrite=0; 
                *ALUSrc=0; 
                *RegWrite=1;
	}
        else if(op==LW_OP||op==LH_OP||op==LHU_OP||op==LB_OP||op==LBU_OP){
                *RegDst=0;
                *ALUSrc=1;
                *MemtoReg=1;
                *RegWrite=1;
                *MemRead=1;
                *MemWrite=0;
                *Branch=0;
                *ALUOp1=0;
                *ALUOp0=0;
        }
        else if(op==SW_OP||op==SH_OP||op==SB_OP){
                *RegDst=0;  //don't care actually
                *ALUSrc=1;
                *MemtoReg=0;  //don't care actually
                *RegWrite=0;
                *MemRead=0;
                *MemWrite=1;
                *Branch=0;
                *ALUOp1=0;  //00 for add directly
                *ALUOp0=0;
        }
        else if(op==BEQ_OP){
                *RegDst=0;  //don't care actually
                *ALUSrc=0;
                *MemtoReg=0;  //din't care actually
                *RegWrite=0;
                *MemRead=0;
                *MemWrite=0;
                *Branch=1;
                *ALUOp1=0;
                *ALUOp0=1;  //01 for sub directly
	}
	else if(op==ADDI_OP||op==ADDIU_OP){
		*RegDst=0;
                *ALUSrc=1;
                *MemtoReg=0;
                *RegWrite=1;
                *MemRead=0;
                *MemWrite=0;
                *Branch=0;
                *ALUOp0=0;
                *ALUOp1=0;  //00 for add directly

	}
	else if(op==LUI_OP||op==ANDI_OP||op==ORI_OP||op==NORI_OP||op==SLTI_OP){
		*RegDst=0;
                *ALUSrc=1;
                *MemtoReg=0;
                *RegWrite=1;
                *MemRead=0;
                *MemWrite=0;
                *Branch=0;
                *ALUOp0=1;
                *ALUOp1=1;
	}
	else if(op==BNE_OP||op==BGTZ_OP){
		*RegDst=0;  //DON'T CARE ACTULLAY
                *ALUSrc=0;
                *MemtoReg=0;
                *RegWrite=0;
                *MemRead=0;
                *MemWrite=0;
                *Branch=1;
                *ALUOp0=1;
                *ALUOp1=1;
	}
	else if(op==J_OP){
		*RegDst=0;
                *ALUSrc=0;
                *MemtoReg=0;
                *RegWrite=0;
                *MemRead=0;
                *MemWrite=0;
                *Branch=0;  //don't care totally notice ALU!
                *ALUOp0=1;
                *ALUOp1=1;
	}
	 else if(op==JAL_OP){
                *RegDst=0;
                *ALUSrc=0;
                *MemtoReg=0;
                *RegWrite=0;
                *MemRead=0;
                *MemWrite=0;
                *Branch=0;  //don't care totally notice ALU!
                *ALUOp0=1;
                *ALUOp1=1;
        }
	else{
	//	printf("%08x",op);
		  //halt here!!!! need modify!!
	}
	 	
	return;
}

void check_Jump(int op,bool* Jump,bool* JAL_sig){
	if(op==J_OP||op==JAL_OP)*Jump=1;
	else *Jump=0;
	if(op==JAL_OP)*JAL_sig=1;
	else *JAL_sig=0;
	return;
}
void check_JR(int op,int funct,bool* JR_sig,bool* RegWrite){
	if(op==0x00){
		if(funct==JR_FUNCT){
			*JR_sig=1;
			*RegWrite=0;
		}		
		else *JR_sig=0;
	}
	else {
		*JR_sig=0;
		return;
	}
}
void RegDst_mul(bool RegDst,int rt,int rd,int* Write_register){
	if(RegDst==0)*Write_register=rt;
	else *Write_register=rd;
	return;
}
void Register(int rs,int rt,int* reg,int* Reg_rdata1,int* Reg_rdata2){
	*Reg_rdata1=reg[rs];
	*Reg_rdata2=reg[rt];
	return;
}
void extend(int C,int* ext_C){
	*ext_C=C<<16;
	*ext_C=*ext_C>>16;
	return;
}

void ALU_control_unit(int op,int funct,int* operation,bool ALUOp1,bool ALUOp0){
	if(ALUOp1==1&&ALUOp0==0){   //check funct
		switch(funct){
		  case ADD_FUNCT:
		  case ADDU_FUNCT:
		    *operation=0;
		    break;
		  case SUB_FUNCT:
		    *operation=1;
		    break;
		  case AND_FUNCT:
		    *operation=2;
		    break;
		  case OR_FUNCT:
		    *operation=3;
		    break;
	  	  case XOR_FUNCT:
		    *operation=4;		
		    break;
		  case NOR_FUNCT:
		    *operation=5;
		    break;
		  case NAND_FUNCT:
		    *operation=6;
		    break;
		  case SLT_FUNCT:
		    *operation=7;
		    break;
		  case SLL_FUNCT:
		    *operation=8;
		    break;
		  case SRL_FUNCT:
            *operation=9;
            break;
          case SRA_FUNCT:
            *operation=10;
            break;
		}			
	}
	else if(ALUOp1==1&&ALUOp0==1){
		switch(op){       //NO JR J JAL
		  case ANDI_OP:
		    *operation=14;
		    break;
          case ORI_OP:
            *operation=15;
            break;
          case NORI_OP:
            *operation=16;
            break;
          case SLTI_OP:
            *operation=7;
            break;
          case LUI_OP:
            *operation=11;
            break;
          case BNE_OP:
            *operation=12;
            break;
          case BGTZ_OP:
            *operation=13;
            break;
		}	
	}
	else if(ALUOp1==0&&ALUOp0==0){
		*operation=0;
	}
	else *operation=1;
	return;
		
}
void ALUSrc_mul(bool ALUSrc,int Reg_rdata2,int ext_C,int* ALU_rdata2){
	if(ALUSrc==0){
		*ALU_rdata2=Reg_rdata2;	
	}
	else *ALU_rdata2=ext_C;
	return;
}
void ALU(int Reg_rdata1,int ALU_rdata2,int operation,int* ALU_result,bool* Branch_Zero,int ext_C,int* shamt,int C){
	
    if(operation==0){
        *ALU_result=Reg_rdata1+ALU_rdata2;
       // printf("R:%08x A:%08x = %08x\n ",Reg_rdata1,ALU_rdata2,Reg_rdata1+ALU_rdata2);
    }
    else if(operation==1){
		*ALU_result=Reg_rdata1-ALU_rdata2;
		if(*ALU_result==0)*Branch_Zero=1;
		else *Branch_Zero=0;
	}
	else if(operation==2)*ALU_result=Reg_rdata1&ALU_rdata2;
	else if(operation==14)*ALU_result=Reg_rdata1&C;
    else if(operation==3)*ALU_result=Reg_rdata1|ALU_rdata2;
    else if(operation==15)*ALU_result=Reg_rdata1|C;
	else if(operation==4)*ALU_result=Reg_rdata1^ALU_rdata2;
	else if(operation==5)*ALU_result=~(Reg_rdata1|ALU_rdata2);
	else if(operation==16)*ALU_result=~(Reg_rdata1|C);
	else if(operation==6)*ALU_result=~(Reg_rdata1&ALU_rdata2);
	else if(operation==7)*ALU_result=(Reg_rdata1<ALU_rdata2);
	else if(operation==8){  //sll
		*shamt=ext_C&0x000007c0;
		*shamt=*shamt<<21;
		*shamt=(unsigned)*shamt>>27;
//printf("shamt:%d \n",*shamt);
		*ALU_result=ALU_rdata2<<*shamt;
	}
	else if(operation==9){    //srl
		*shamt=ext_C&0x000007c0;
                *shamt=*shamt<<21;
                *shamt=(unsigned)*shamt>>27;
                *ALU_result=(unsigned)ALU_rdata2>>*shamt;

	}
	else if(operation==10){    //srl
		*shamt=ext_C&0x000007c0;
                *shamt=*shamt<<21;
                *shamt=(unsigned)*shamt>>27;
                *ALU_result=ALU_rdata2>>*shamt;
	}
	else if(operation==11){
		*ALU_result=ALU_rdata2<<16;
	}
	else if(operation==12){  //bne
        *ALU_result=Reg_rdata1-ALU_rdata2;
		if(Reg_rdata1!=ALU_rdata2)*Branch_Zero=1;
		else *Branch_Zero=0;
	}
	else if(operation==13){
		if(Reg_rdata1>0)*Branch_Zero=1;
		else *Branch_Zero=0;
	}
	else printf("BUG operation bigger than 13");
	return;
}
void Memory(bool MemRead,bool MemWrite,int ALU_result,int* Mem_rdata,int Reg_rdata2, int* DMemory,int op){
	int i;
	int tempt=0;
	int use;
	*Mem_rdata=0;
	if(MemRead==1){
		if(op==LW_OP){
			for(i=0;i<4;i++){
				use=ALU_result+i;
				*Mem_rdata=DMemory[use]+(*Mem_rdata<<8);
			}
		}
		if(op==LH_OP){
                        for(i=0;i<2;i++){
                                tempt=DMemory[ALU_result+i]+(tempt<<8);
                        }
			tempt=(tempt<<16);
                        tempt=(tempt>>16);
                        *Mem_rdata=tempt;
                }
		if(op==LHU_OP){
                        for(i=0;i<2;i++){
                                tempt=DMemory[ALU_result+i]+(tempt<<8);
                        }
			tempt=tempt<<16;
                        tempt=(unsigned)tempt>>16;
                        *Mem_rdata=tempt;
                }
		if(op==LB_OP){
                        for(i=0;i<1;i++){
                                tempt=DMemory[ALU_result+i];
                                tempt=tempt<<24;
                                tempt=tempt>>24;
                                *Mem_rdata=tempt;
                        }
                }
		 if(op==LBU_OP){
                        for(i=0;i<1;i++){
                                tempt=DMemory[ALU_result+i];
                                tempt=tempt<<24;
                                tempt=(unsigned)tempt>>24;
                                *Mem_rdata=tempt;
                        }
                }				
	}
	tempt=0;
	if(MemWrite==1){
		if(op==SW_OP){
			tempt=Reg_rdata2&0xff000000;
			DMemory[ALU_result]=(unsigned)tempt>>24;
			tempt=Reg_rdata2&0x00ff0000;
            DMemory[ALU_result+1]=(unsigned)tempt>>16;
			tempt=Reg_rdata2&0x0000ff00;
            DMemory[ALU_result+2]=(unsigned)tempt>>8;
			tempt=Reg_rdata2&0x000000ff;
            DMemory[ALU_result+3]=(unsigned)tempt;
           
		}
		if(op==SH_OP){
                        tempt=Reg_rdata2&0x0000ff00;
                        DMemory[ALU_result]=(unsigned)tempt>>8;
                        tempt=Reg_rdata2&0x000000ff;
                        DMemory[ALU_result+1]=(unsigned)tempt;
                }
		if(op==SB_OP){
                        tempt=Reg_rdata2&0x000000ff;
                        DMemory[ALU_result]=(unsigned)tempt;
                }


	}
}
void Mem_mul(bool JAL_sig,int pc,int* reg,int Write_register,int RegWrite,int MemtoReg,int ALU_result,int Mem_rdata){
	if(JAL_sig==1)reg[31]=pc+4;
	if(MemtoReg==1&&RegWrite==1&&Write_register!=0){
        reg[Write_register]=Mem_rdata;
	}
	
	if(MemtoReg==0&&RegWrite==1&&Write_register!=0){
                reg[Write_register]=ALU_result;
	
	}
}
int PC_control(int *pc,bool Branch,bool Jump,bool Branch_Zero,bool JR_sig,int ins,int ext_C,int or_pc,int Reg_rdata1){
	int first_mul;
	int tempt;
	int pc_front;
	int jump_addr;
	int i;
	*pc=*pc+4;
	if(Branch==1&&Branch_Zero==1){
		first_mul=(ext_C<<2)+*pc;
//printf("%08x  %08x\n",*pc,first_mul);
	}
	else first_mul=*pc;
	if(Jump==1){
		tempt=ins&0x03ffffff;
		tempt=tempt<<2;
		pc_front=*pc&0xf0000000;
		jump_addr=tempt|pc_front;
		*pc=jump_addr;
	}
	else if(JR_sig==1){
		*pc=Reg_rdata1;
	}
	else *pc=first_mul;
	i= (*pc-or_pc)/4;
	return i;
}
	
void print_result(FILE* fp,int* reg,int pc,int* cycle_count){
	int i;
	fprintf(fp,"cycle %d\n",*cycle_count);
	for(i=0;i<32;i++){
		fprintf (fp,"$%02d: 0x%08X\n",i,reg[i]);
  	 }
    fprintf(fp,"PC: 0x%08X\n\n\n",pc);    
	*cycle_count=*cycle_count+1;	
}
	
void Error_Detect(FILE *file_err,bool RegWrite,int Write_register,int funct,int op,int rt,int shamt,int pc,int or_pc,int cycle_counter,int Reg_rdata1,int ALU_rdata2,int ALU_result,int operation,bool* error_halt_ptr){
    *error_halt_ptr=0;
    if(RegWrite==1&& Write_register==0){   //write to register 0
//printf("op:%08x funct:%08x\n",op,funct);
        if(op!=SLL_OP||funct!=SLL_FUNCT){
            fprintf(file_err,"In cycle %d: Write $0 Error\n",cycle_counter);    
        }
        else{
            if(shamt==0&&rt==0){}
            else{
                fprintf(file_err,"In cycle %d: Write $0 Error\n",cycle_counter);                
            }
        }
    }
    if(operation==0){    //number overflow 
        //printf("ALU_rdata2:%08x\n",ALU_rdata2);
            if(Reg_rdata1>0&&ALU_rdata2>0&&ALU_result<=0){
                if(op!=ADDIU_OP&&(op!=ADDU_OP||funct!=ADDU_FUNCT)){
                     fprintf(file_err,"In cycle %d: Number Overflow\n",cycle_counter);
                    }
            }
            else if(Reg_rdata1<0&&ALU_rdata2<0&&ALU_result>=0){
                if(op!=ADDIU_OP&&(op!=ADDU_OP||funct!=ADDU_FUNCT)){
                     fprintf(file_err,"In cycle %d: Number Overflow\n",cycle_counter);
                }
            }
    }
    else if(operation==1&&op!=BEQ_OP){
            if(Reg_rdata1>0&&ALU_rdata2<0&&ALU_result<=0){
                if(ALU_rdata2!=0x80000000){
                     fprintf(file_err,"In cycle %d: Number Overflow\n",cycle_counter);                
                 }
            }
            else if(Reg_rdata1<0&&ALU_rdata2>0&&ALU_result>=0){
                     fprintf(file_err,"In cycle %d: Number Overflow\n",cycle_counter);                
            }
            else if(Reg_rdata1<0&&ALU_rdata2==0x80000000){    
                     fprintf(file_err,"In cycle %d: Number Overflow\n",cycle_counter);                
            }
         
    }
    if(op==LW_OP||op==SW_OP){                   //address overflow
        if(ALU_result>1020||ALU_result<0){
            fprintf(file_err , "In cycle %d: Address Overflow\n", cycle_counter);
            *error_halt_ptr=1;
        }
        if((ALU_result%4)!=0){
            fprintf(file_err, "In cycle %d: Misalignment Error\n", cycle_counter);
            *error_halt_ptr=1;
        }
    } 
    else if(op==LH_OP||op==LHU_OP||op==SH_OP){
        if(ALU_result>1022||ALU_result<0){
            fprintf(file_err , "In cycle %d: Address Overflow\n", cycle_counter);
            *error_halt_ptr=1;
        }
        if((ALU_result%2)!=0){
            fprintf(file_err , "In cycle %d: Misalignment Error\n", cycle_counter);
            *error_halt_ptr=1;
        }
    }
    else if(op==LB_OP||op==LBU_OP||op==SB_OP){
        if(ALU_result>1023||ALU_result<0){
            fprintf(file_err , "In cycle %d: Address Overflow\n", cycle_counter);
            *error_halt_ptr=1;
        }
    }
}
    
void PC_Overflow_Detect(FILE *file_err,int pc,bool* error_halt_ptr,int cycle_count){
    *error_halt_ptr=0;
    if(pc>1023||pc<0){
        fprintf(file_err , "In cycle %d: Address Overflow\n", cycle_count);
        *error_halt_ptr=1;
    }
}
int main(){
	int pc,or_pc;
	int reg[32],DMemory[1024];
	int ins[500];
	int i;
	for(i=0;i<32;i++){reg[i]=0;}
	for(i=0;i<500;i++){ins[i]=0;}
	for(i=0;i<1024;i++){DMemory[i]=0;}
	int op,rs,rt,rd,C,funct,shamt;
	bool JAL_sig,Jump,JR_sig,RegDst,ALUSrc,MemtoReg,RegWrite,MemRead,MemWrite,Branch,ALUOp1,ALUOp0,Branch_Zero;
	bool error_halt_ptr;
    int cycle_count=0;
	int Write_register,Reg_rdata1,Reg_rdata2,ext_C,operation,ALU_rdata2,ALU_result,Mem_rdata;
	FILE *file_i;
	FILE *file_d;
	FILE *file_wr;
    FILE *file_err;
    file_err=fopen("error_dump.rpt","w");
	file_i=fopen("iimage.bin", "rb");
	file_d=fopen("dimage.bin","rb");
	file_wr=fopen("snapshot.rpt","w");
	open_i(file_i,&or_pc,ins);
	open_d(file_d,reg,DMemory);
	fclose(file_i);
	fclose(file_d);
	int counter=0;
	pc=or_pc;
	print_result(file_wr,reg,pc,&cycle_count);
	while(1){
       // printf("cycle:%d\n",cycle_count);
		divide_instruction(ins[counter],&op,&rs,&rt,&rd,&C,&funct);
        if(op==0x3f)break;
       // printf("op:%06x funct:%06x\n",op,funct);		
		signal(op,&RegDst,&ALUSrc,&MemtoReg,&RegWrite,&MemRead,&MemWrite,&Branch,&ALUOp1,&ALUOp0);
			
		check_Jump(op,&Jump,&JAL_sig);

		check_JR(op,funct,&JR_sig,&RegWrite);
        //printf("RegWrite:%d\n",RegWrite);
		RegDst_mul(RegDst,rt,rd,&Write_register);

       // printf("RegWrite:%d\n",RegWrite);
		RegDst_mul(RegDst,rt,rd,&Write_register);
		Register(rs,rt,reg,&Reg_rdata1,&Reg_rdata2);
	   // printf("Reg_rdata1:%08x Reg_rata2:%08x\n",Reg_rdata1,Reg_rdata2);	
		extend(C,&ext_C);

       // printf("op:%06x funct:%06x\n",op,funct);		
		//signal(op,&RegDst,&ALUSrc,&MemtoReg,&RegWrite,&MemRead,&MemWrite,&Branch,&ALUOp1,&ALUOp0);
		ALU_control_unit(op,funct,&operation,ALUOp1,ALUOp0);
       // printf("Reg_rdata2:%08x ALU_rdata2:%08x operation:%d \n",Reg_rdata2,ALU_rdata2,operation);
		ALUSrc_mul(ALUSrc,Reg_rdata2,ext_C,&ALU_rdata2);
       // printf("Reg_rdata2:%08x ALU_rdata2:%08x operation:%d\n",Reg_rdata2,ALU_rdata2,operation);
		ALU(Reg_rdata1,ALU_rdata2,operation,&ALU_result,&Branch_Zero,ext_C,&shamt,C);
       // printf("ALU_result:%08x Branch_Zero:%d\n",ALU_result,Branch_Zero);
       // printf("RegWrite=%d Write_register=%08x\n ",RegWrite,Write_register);
        Error_Detect(file_err,RegWrite,Write_register,funct,op,rt,shamt,pc,or_pc,cycle_count,Reg_rdata1,ALU_rdata2,ALU_result,operation,&error_halt_ptr);
        if(error_halt_ptr==1)break;

        Memory(MemRead,MemWrite,ALU_result,&Mem_rdata,Reg_rdata2,DMemory,op);

		Mem_mul(JAL_sig,pc,reg,Write_register,RegWrite,MemtoReg,ALU_result,Mem_rdata);
       // printf("Branch=%d Branch_zero=%d\n\n",Branch,Branch_Zero);
		counter=PC_control(&pc,Branch,Jump,Branch_Zero,JR_sig,ins[counter],ext_C,or_pc,Reg_rdata1);
		print_result(file_wr,reg,pc,&cycle_count);
        PC_Overflow_Detect(file_err,pc,&error_halt_ptr,cycle_count);
        if(error_halt_ptr==1)break;
        if(counter<0)counter=499;
	}

	fclose(file_wr);
    fclose(file_err);
    return 0;
}


#define  EE_GROUPS 			16
#define  FEE_GROUPS  		48

#define  EE_REGS 			256
#define  FEE_REGS 			768

#define EEGRP_COMM	0x01
#define EE_CLA				0x0010
#define EE_CLT				0x0011

#define EE_GLA				0x0018
#define EE_GLT				0x0019

#define EEGRP_DIO	0x03
#define EE_MDI_BASE			0x0030	
#define EE_CDI_BASE			0x0034
#define EE_HDI_BASE			0x0036	

#define EEGRP_LIFT	0x04
#define	EE_VRAT				0x0040	// EE Lift Running Data
#define EE_PPR				0x0041
#define EE_RPM				0x0042
#define EE_INV				0x0043

#define EE_STV				0x0045	// EE Velocity Generator Data
#define EE_ACR				0x0046
#define EE_DER				0x0047
#define EE_JSR				0x0048
#define EE_JTS				0x0049
#define EE_JDR				0x004A
#define EE_LTG				0x004B
#define EE_FSR				0x004C
#define EE_DTO				0x004D
#define EE_DTD				0x004E
#define EE_LVE				0x004F

#define EEGRP_MOTION	0x05
#define EE_VLD				0x0050	// EE Motion Data
#define EE_VIN				0x0051
#define EE_VTH				0x0052
#define EE_VFD				0x0053
#define EE_VZR				0x0054

#define EE_MUL				0x005C
#define EE_MLL				0x005D
#define EE_VEE				0x005E
#define EE_PLE				0x005F

#define EEGRP_SFLR	0x06
#define EE_LBY				0x0060	// EE Special floor
#define EE_HM1				0x0061
#define EE_FIR				0x0062

#define EEGRP_DRV	0x07			// Group 7 Driver control Parameter
#define EE_BPT				0x0071	
#define EE_BPH				0x0072	
#define EE_BDT				0x0073	
#define EE_MDT				0x0074	

#define EE_BDL				0x007D	
#define EE_BTO				0x007E	
#define EE_MTO				0x007F	

#define EEGRP_DOOR	0x08			// Group 8 Door control Parameter
#define EE_FDS				0x0080	
#define EE_RDS				0x0081	
#define EE_POE				0x0082	
#define EE_ZON				0x0083	

#define EE_CDT				0x0084
#define EE_CHT				0x0085
#define EE_SDT				0x0086
#define EE_LDT				0x0087

#define EE_ADA				0x0088

#define EE_DOP				0x008B	// Door Open Protective time
#define EE_DCP				0x008C	// Door Close Protective time
#define EE_DHT				0x008D	// Door Hold Time
#define EE_DOT				0x008E	// Door Open Time-out (input devices)
#define EE_DCC				0x008F	// Door Cycle count

#define EEGRP_FCW	0x0A
#define EE_FCW_BASE			0x00A0	// EE Special floor

#define EE_CHAR_BASE		0x00C0 //  Floor Indicating Character

/*
*********************************************************************************************************
*********************************************************************************************************
*/
#define FEE_POSI_BASE		0x0100
#define FEE_DLVE_BASE		0x0180
#define FEE_DENT_BASE		0x0200
#define FEE_ULVE_BASE		0x0280
#define FEE_UENT_BASE		0x0300 

#define FEE_PPLU_BASE		0x0380 
#define FEE_PPLD_BASE		0x0382 
#define FEE_PPLUD_BASE		0x0384
#define FEE_PPLDD_BASE		0x0386
#define FEE_PPLUM_BASE		0x0388
#define FEE_PPLDM_BASE		0x038A

#define FEGRP_TEACH	0x3F
#define FEE_TSR				0x03F0	
#define FEE_FLR				0x03F2	
	
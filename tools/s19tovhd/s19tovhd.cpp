/*
    Copyright (c) 2011, Meldora Inc.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following
    conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
      in the documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
    OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
    OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

/*
    This is an adaptation of the tool S19ToVHD from the System09 project.

    Contributors:
        (The original author(s) was/were unfortunately not specified in the source code)
        Daniel Cliche (dcliche@meldora.com)
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/*
* equates
*/
#define EPROM_MAX (1<<16)
#define CMD_LINE_MAX 80
#define FALSE 0
#define TRUE !FALSE
#define BINARY 0
#define MOTOROLA 1
#define INTEL 2
#define SMAL32 3
#define VHDL_BIN 4
#define VHDL_BYTE 5
#define VHDL_WORD 6

/*
* global variables
*/
FILE *cmdfp;				/* command input pointer */
char cmdbuff[CMD_LINE_MAX];
unsigned char eprom_buff[EPROM_MAX];	/* eprom buffer */
int eprom_top;				/* top of EPROM buffer */
int mod_flag;				/* buffer has been modified */
int auxflag;				/* Auxillary input file specified */
int count;
int checksum;
int offset;				/* Eprom Buffer memory offset */
int format_type;			/* load / save format type */
char *hex_str = "0123456789ABCDEF";


/*
* compare a string of specified length
* return TRUE if a match
* return FALSE if no match
* ignore case
*/
int str_equal( char *s1, char *s2, int len )
{
	int i = 0;
	while( i<len ) {
		if( toupper( s1[i] ) == toupper( s2[i] ) )
			i++;
		else
			return FALSE;
	}
	return TRUE;
}	

int to_hexadecimal( char c )
{
	for( int k=0; k<16; k++ ) {
		if( toupper(c) == hex_str[k] ) return k;
	}
	return -1;
}			

/*
* extract an address from the command line
* returns an offset to the end of the argument.
*/
int get_address( char *cb, int *addr )
{
	int i, j, k;

	j = i = 0;

	while((k = to_hexadecimal(cb[i])) != -1) {
		i++;
		j = j *16 + k;
	}
	*addr = j;
	if( i == 0 ) return i;
	while( isspace( cb[i]) ) i++;
	return i;
}

/*
* Motorola S1 format to Intel hex format
* Usage
* mot2hex <file_name>
*/

int gethex( FILE *fp_in )
{
	int hex;

	hex = fgetc( fp_in );
	return( to_hexadecimal( hex ) );
}

int get2hex( FILE *fp_in )
{
	int hexhi, hexlo, byte;

	hexhi = gethex( fp_in );
	if( hexhi != -1 )
	{
		hexlo = gethex( fp_in );
		if( hexlo != -1 )
		{
			byte = hexhi * 16 + hexlo;
			checksum = (checksum + byte) & 0xff;
			return byte;
		}
	}
	return -1;
}

int get4hex( FILE *fp_in )
{
	int bytehi, bytelo, addr;

	bytehi = get2hex( fp_in );
	if( bytehi != -1 )
	{
		bytelo = get2hex( fp_in );
		if( bytelo != -1 )
		{
			addr = (bytehi * 256) + bytelo;
			return addr;
		}
	}
	return -1;
}    

int get6hex( FILE *fp_in )
{
	int bytehi, bytemid, bytelow, addr;

	bytehi = get2hex( fp_in );
	if( bytehi != -1 )
	{
		bytemid = get2hex( fp_in );
		if( bytemid != -1 )
		{
			bytelow = get2hex( fp_in );
			if( bytelow != -1 )
			{
				addr = (bytehi << 16) + (bytemid << 8) + bytelow;
				return addr;
			}
		}
	}
	return -1;
}    

long get8hex( FILE *fp_in )
{
	int wordhi, wordlow;
	long addr;

	wordhi = get4hex( fp_in );
	if( wordhi != -1 )
	{
		wordlow = get4hex( fp_in );
		if( wordlow != -1 )
		{
			addr = ((long)wordhi << 16) + (long)wordlow;
			return addr;
		}
	}
	return -1;
}

/*
* load motorola formatted file
*/

bool load_mot( char *fname_in )
{
	FILE *fp_in;
	int byte, addr, i;

	fp_in = fopen( fname_in, "r" );
	if ( !fp_in ) {
		printf( "\nCan't open %s", fname_in );
		return false; 
	}

	byte = 0;
	addr = 0;

	while( byte != -1 ) {
		do {
			byte = fgetc( fp_in);
		} while( (byte != 'S') && (byte != -1) );

		byte = fgetc( fp_in );
		checksum = 0;
		if ( (byte == '1') || (byte == '2') ) {
			count = get2hex( fp_in );
			if ( byte == '1' ) {
				addr = get4hex( fp_in );
				count -= 3;
			} else {
				addr = get6hex( fp_in );
				count -= 4;
			}
			for( i=0; i<count; i++ ) {
				byte = get2hex( fp_in );
				eprom_buff[( addr - offset) % EPROM_MAX ] = (unsigned char)byte;
				addr++;
			}
			byte = get2hex( fp_in);
			checksum = (~checksum) & 0xff;
			if ( checksum != 0 )
				printf( "\nchecksum error - read check = %02x", byte );
		}
	}
	fclose( fp_in );
	return true;
}

int put2hex( FILE *fp, int h )
{
	int hex = (h & 0xf0)>>4;
	int i = fputc( (int)hex_str[hex], fp );
	hex = (h & 0xf);
	i = fputc( (int)hex_str[hex], fp );
	checksum = (checksum + h) & 0xff;
	return i;
}

int put4hex( FILE * fp, int h )
{
	int i = put2hex( fp, (h & 0xff00 )>>8 );
	i = put2hex( fp, (h & 0xff) );
	return i;
}

char *bin_string( int num, int num_len )
{
	static char retbuf[33];
	char *p;
	int i;

	p = &retbuf[sizeof(retbuf)-1];
	*p = '\0';
	for (i=0; i<num_len; i++ ) {
		*--p = "01"[num % 2];
		num >>= 1;
	} 
	return p;
}

/*
* save VHDL hexadecimal file 4KBit Block RAM (Spartan 2)
*/

void save_vhdl_b4( FILE *fp_out, char *entity_name, int start_addr, int end_addr )
{
	int addr;
	int i,j;
	int byte;
	int rom_num, rom_max, rom_len;
	int addr_len;

	rom_max = (end_addr+1-start_addr)/512;
	rom_len = 8;
	addr_len = (int)(log((double)(end_addr-start_addr))/log(2.0));

	fprintf(fp_out, "library IEEE;\n");
	fprintf(fp_out, "   use IEEE.std_logic_1164.all;\n");
	fprintf(fp_out, "   use IEEE.std_logic_arith.all;\n");
	fprintf(fp_out, "library unisim;\n");
	fprintf(fp_out, "   use unisim.vcomponents.all;\n");
	fprintf(fp_out, "\n");
	fprintf(fp_out, "entity %s is\n", entity_name);
	fprintf(fp_out, "   port(\n");
	fprintf(fp_out, "      clk       : in  std_logic;\n");
	fprintf(fp_out, "      rst       : in  std_logic;\n");
	fprintf(fp_out, "      cs        : in  std_logic;\n");
	fprintf(fp_out, "      rw        : in  std_logic;\n");
	fprintf(fp_out, "      addr      : in  std_logic_vector(%d downto 0);\n", addr_len);
	fprintf(fp_out, "      data_out  : out std_logic_vector(7 downto 0);\n");
	fprintf(fp_out, "      data_in   : in  std_logic_vector(7 downto 0)\n");
	fprintf(fp_out, "   );\n");
	fprintf(fp_out, "end %s;\n", entity_name);
	fprintf(fp_out, "\n");
	fprintf(fp_out, "architecture rtl of %s is\n", entity_name);
	fprintf(fp_out, "\n");
	fprintf(fp_out, "   type data_array is array(0 to %d) of std_logic_vector(7 downto 0);\n",rom_max-1);
	fprintf(fp_out, "   signal xdata : data_array;\n");
	fprintf(fp_out, "   signal en : std_logic_vector(%d downto 0);\n", rom_max-1);
	fprintf(fp_out, "   signal we : std_logic;\n");
	fprintf(fp_out, "\n");
	fprintf(fp_out,"   begin\n");
	fprintf(fp_out, "\n");

	addr=start_addr;
	for( rom_num=0; rom_num<rom_max; rom_num++) {
		fprintf(fp_out, "   ROM%02x: RAMB4_S8\n", rom_num );
		fprintf(fp_out, "      generic map (\n");
		for( j=0; j<16; j++ ) {
			fprintf( fp_out, "         INIT_%02x => x\"", j );
			for(i=31; i>=0; i-- ) {
				byte = (int)eprom_buff[(addr - offset + i) % EPROM_MAX];
				putc( hex_str[(byte >>4) & 0xf], fp_out ); 
				putc( hex_str[byte & 0xf], fp_out ); 
			}
			if (j < 15) {
				fprintf( fp_out, "\",\n" );
			} else {
				fprintf( fp_out, "\"\n" );
			}
			addr+=32;
		}
		fprintf(fp_out, "      )\n");
		fprintf(fp_out, "      port map (\n");
		fprintf(fp_out, "         clk     => clk,\n");
		fprintf(fp_out, "         en      => en(%d),\n", rom_num );
		fprintf(fp_out, "         we      => we,\n");
		fprintf(fp_out, "         rst     => rst,\n");
		fprintf(fp_out, "         addr    => addr(8 downto 0),\n");
		fprintf(fp_out, "         di      => data_in,\n");
		fprintf(fp_out, "         do      => xdata(%d)\n", rom_num );
		fprintf(fp_out, "      );\n");
		fprintf(fp_out, "\n");
	}

	fprintf(fp_out, "   rom_glue: process (cs, rw, addr, xdata)\n");
	fprintf(fp_out, "   begin\n");
	if( addr_len > rom_len ) {
		fprintf(fp_out, "      en <= (others=>'0');\n");
		fprintf(fp_out, "      case addr(%d downto %d) is\n", addr_len, rom_len+1 );

		for( rom_num=0; rom_num<rom_max; rom_num++ ) {
			fprintf(fp_out, "      when \"%s\" =>\n", bin_string(rom_num, addr_len-rom_len) );
			fprintf(fp_out, "         en(%d)  <= cs;\n", rom_num );
			fprintf(fp_out, "         data_out  <= xdata(%d);\n", rom_num);
		}

		fprintf(fp_out, "      when others =>\n");
		fprintf(fp_out, "         null;\n");
		fprintf(fp_out, "      end case;\n");
	} else {
		fprintf(fp_out, "      en(0)  <= cs;\n" );
		fprintf(fp_out, "      data_out  <= xdata(0);\n" );
	}
	fprintf(fp_out, "      we <= not rw;\n");
	fprintf(fp_out, "   end process;\n");
	fprintf(fp_out, "end architecture rtl;\n\n");
}


/*
* save VHDL hexadecimal file 16 KBit Block RAM (Spartan 3)
*/

void save_vhdl_b16( FILE *fp_out, char *entity_name, int start_addr, int end_addr )
{
	int addr;
	int i,j;
	int byte;
	int rom_num, rom_max, rom_len;
	int addr_len;

	rom_max = (end_addr+1-start_addr)/2048;
	rom_len = 10;
	addr_len = (int)(log((double)(end_addr-start_addr))/log(2.0));

	fprintf(fp_out, "library IEEE;\n");
	fprintf(fp_out, "   use IEEE.std_logic_1164.all;\n");
	fprintf(fp_out, "   use IEEE.std_logic_arith.all;\n");
	fprintf(fp_out, "library unisim;\n");
	fprintf(fp_out, "   use unisim.vcomponents.all;\n");
	fprintf(fp_out, "\n");
	fprintf(fp_out, "entity %s is\n", entity_name);
	fprintf(fp_out, "   port(\n");
	fprintf(fp_out, "      clk       : in  std_logic;\n");
	fprintf(fp_out, "      rst       : in  std_logic;\n");
	fprintf(fp_out, "      cs        : in  std_logic;\n");
	fprintf(fp_out, "      rw        : in  std_logic;\n");
	fprintf(fp_out, "      addr      : in  std_logic_vector(%d downto 0);\n", addr_len);
	fprintf(fp_out, "      data_out  : out std_logic_vector(7 downto 0);\n");
	fprintf(fp_out, "      data_in   : in  std_logic_vector(7 downto 0)\n");
	fprintf(fp_out, "   );\n");
	fprintf(fp_out, "end %s;\n", entity_name);
	fprintf(fp_out, "\n");
	fprintf(fp_out, "architecture rtl of %s is\n", entity_name);
	fprintf(fp_out, "\n");
	fprintf(fp_out, "   type data_array is array(0 to %d) of std_logic_vector(7 downto 0);\n",rom_max-1);
	fprintf(fp_out, "   signal xdata : data_array;\n");
	fprintf(fp_out, "   signal en : std_logic_vector(%d downto 0);\n", rom_max-1);
	fprintf(fp_out, "   signal dp : std_logic_vector(%d downto 0);\n", rom_max-1);
	fprintf(fp_out, "   signal we : std_logic;\n");
	fprintf(fp_out, "\n");
	fprintf(fp_out, "   begin\n");
	fprintf(fp_out, "\n");

	addr=start_addr;
	for( rom_num=0; rom_num<rom_max; rom_num++) {
		fprintf(fp_out, "   ROM%02x: RAMB16_S9\n", rom_num );
		fprintf(fp_out, "      generic map (\n");
		for( j=0; j<64; j++ ) {
			fprintf( fp_out, "         INIT_%02x => x\"", j );
			for(i=31; i>=0; i-- ) {
				byte = (int)eprom_buff[(addr - offset + i) % EPROM_MAX];
				putc( hex_str[(byte >>4) & 0xf], fp_out ); 
				putc( hex_str[byte & 0xf], fp_out ); 
			}
			if (j < 63)  {
				fprintf( fp_out, "\",\n" );
			} else {
				fprintf( fp_out, "\"\n" );
			}
			addr+=32;
		}
		fprintf(fp_out, "      )\n");
		fprintf(fp_out, "      port map (\n");
		fprintf(fp_out, "         CLK     => clk,\n");
		fprintf(fp_out, "         SSR     => rst,\n");
		fprintf(fp_out, "         EN      => en(%d),\n", rom_num );
		fprintf(fp_out, "         WE      => we,\n");
		fprintf(fp_out, "         ADDR    => addr(10 downto 0),\n");
		fprintf(fp_out, "         DI      => data_in,\n");
		fprintf(fp_out, "         DIP(0)  => dp(%d),\n", rom_num );
		fprintf(fp_out, "         DO      => xdata(%d),\n", rom_num );
		fprintf(fp_out, "         DOP(0)  => dp(%d)\n", rom_num );
		fprintf(fp_out, "      );\n");
	}

	fprintf(fp_out, "   rom_glue: process (cs, rw, addr, xdata)\n");
	fprintf(fp_out, "   begin\n");
	if( addr_len > rom_len ) {
		fprintf(fp_out, "      en <= (others=>'0');\n");
		fprintf(fp_out, "      case addr(%d downto %d) is\n", addr_len, rom_len+1 );

		for( rom_num=0; rom_num<rom_max; rom_num++ ) {
			fprintf(fp_out, "      when \"%s\" =>\n", bin_string(rom_num, addr_len-rom_len) );
			fprintf(fp_out, "         en(%d)  <= cs;\n", rom_num );
			fprintf(fp_out, "         data_out  <= xdata(%d);\n", rom_num);
		}

		fprintf(fp_out, "      when others =>\n");
		fprintf(fp_out, "         null;\n");
		fprintf(fp_out, "      end case;\n");
	} else {
		fprintf(fp_out, "      en(0)  <= cs;\n");
		fprintf(fp_out, "      data_out  <= xdata(0);\n");
	}
	fprintf(fp_out, "      we <= not rw;\n");
	fprintf(fp_out, "   end process;\n");
	fprintf(fp_out, "end architecture rtl;\n\n");
}

/*
* epedit main program
*/
int main(int argc, char* argv[])
{
	int start_addr;
	int end_addr;
	int arglen;
	char entity_name_buf[512];
	FILE *fp_out;

	if (argc < 5)
	{
		printf("Usage: s19tovhd <bram_type> <s19 file> <base vhd file> <vhdl base entity name> <addr> [<addr> ...]\n");
		return(-1);
	}
	int bram_type_arg_pos = 1;
	int s19_file_arg_pos = 2;
	int base_vhdl_file_arg_pos = 3;
	int entity_name_arg_pos = 4;
	int first_addr_arg_pos = 5;

	char *bram_type = argv[bram_type_arg_pos];
	char *s19_file = argv[s19_file_arg_pos];
	char *base_vhd_file = argv[base_vhdl_file_arg_pos];
	char *entity_name = argv[entity_name_arg_pos];
	printf("Block-RAM type '%s'\n", bram_type);
	printf("Reading Motorola S19 from file '%s'\n", s19_file);
	printf("VHDL file name '%s'\n", base_vhd_file);
	printf("Base RAM/ROM entity name is '%s'\n", entity_name);
	if (! load_mot( s19_file )) return (-1);

	if (strcmp(bram_type,"b16") == 0) {
		if( (fp_out = fopen( base_vhd_file, "w" )) == NULL ) {
			printf( "\nCan't open '%s' for write ", base_vhd_file );
			return(-1);
		}

		for (int cnt=first_addr_arg_pos; cnt<argc; cnt++) {
			if( (arglen = get_address( argv[cnt], &start_addr )) == 0 ) {
				printf("Expected hex start address, got %s\n", argv[cnt]);
				continue;
			}
			end_addr = start_addr + 2047;
			sprintf(entity_name_buf, "%s_%4X", entity_name, start_addr);

			printf("Entity '%s' (address range '0x%4X'-'0x%4X') written to file '%s'\n", 
				entity_name_buf, start_addr, end_addr, base_vhd_file);
			save_vhdl_b16( fp_out, entity_name_buf, start_addr, end_addr );
		}
		if (fp_out) fclose(fp_out);
	}
	if (strcmp(bram_type,"b4") == 0) {
		if( (fp_out = fopen( base_vhd_file, "w" )) == NULL ) {
			printf( "\nCan't open '%s' for write ", base_vhd_file );
			return(-1);
		}

		for (int cnt=first_addr_arg_pos; cnt<argc; cnt++) {
			if( (arglen = get_address( argv[cnt], &start_addr )) == 0 ) {
				printf("Expected hex start address, got %s\n", argv[cnt]);
				continue;
			}
			end_addr = start_addr + 2047;
			sprintf(entity_name_buf, "%s_%4X", entity_name, start_addr);

			printf("Entity '%s' (address range '0x%4X'-'0x%4X') written to file '%s'\n", 
				entity_name_buf, start_addr, end_addr, base_vhd_file);
			save_vhdl_b4( fp_out, entity_name_buf, start_addr, end_addr );
		}
		if (fp_out) fclose(fp_out);
	}
	return(0);
}


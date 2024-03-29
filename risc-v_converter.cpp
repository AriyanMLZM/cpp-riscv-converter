/* 
	Ariyan Molazem
	This program gets the RISC-V code and converts it to the machine code.
*/
#include <iostream>
#include <conio.h>
#include <cmath>
#include <sstream>
using namespace std;     
/////////////////////////////////////////////////// (Main Variables)
string input_code, parts[4],                                     
	   opcode,                                   
	   funct3,                           
	   funct7,
	   imm_12, imm_p1, imm_p2,
	   rd,
	   rs1,
	   rs2,
	   binary_code,
	   hex_code,
	   list_r_op[10], list_i_op[9], list_l_op[7], list_s_op[4], 
	   list_register[32];                                 
int op_code, format_code;
bool checked, exit_prog;
/////////////////////////////////////////////////// (Functions) 
void line();                                     // Prints a line.
void make_lists();                               // Makes the op & reg list.

void get_input();                                // Gets the input code.
void tokenizer();                                // Separates the input.
void analyse_parts();                            // Works on the separated parts. 
void print_results();                            // Shows the final results.

void check_op();                                 // Checks op for validity.
void check_register(string reg);                 // Checks registers for validity.

void r_format();
void i_format();
void s_format();

void make_register(int select, string reg);
void make_imm_12();
void make_hex_code();

string convert_to_binary(int number);            // Converts decimal to binary.
int convert_to_decimal(string number);           // Converts binary to deciaml.
string convert_to_hex(int num);                  // Converts to hex.
/////////////////////////////////////////////////// (Main)
int main()
{ 
	exit_prog = false;
	make_lists();
	while(true)
	{
		system("cls");
		get_input();
		if(exit_prog)
			break;
		tokenizer();
		
		analyse_parts();
		if(!checked)
		{
			cout << "\n [Invalid input] "
					"\n o Enter like the example."
					"\n o Pay attention to the range of registers."
			        "\n\n press any key to try again";
			_getch();
			continue;
		}
		
		switch(format_code)
		{
			case 1:
				binary_code = funct7 + rs2 + rs1 + funct3 + rd + opcode;
				break;
			case 2:
				binary_code = imm_12 + rs1 + funct3 + rd + opcode;
				break;
			case 4:
				binary_code = imm_p2 + rs2 + rs1 + funct3 + imm_p1 + opcode;
		}
		make_hex_code();
		
		system("cls");
		print_results();
	}
	return 0;
}
/////////////////////////////////////////////////// Functions
void get_input()
{
	cout << "\n                 < R, I, S - format >";
	line();
	cout << " Notes"
			"\n\n o This Program Converts RISC-V code to Machine code."
			"\n\n o Supported codes:\n\n   R-F: ";
	for(int i = 0; i != 10; i++)		
		cout << list_r_op[i] << " ";
	cout << "\n\n   I-F: ";
	for(int i = 0; i != 9; i++)
	{		
		cout << list_i_op[i] << " ";
	}
	cout << "\n        ";
	for(int i = 0; i != 7; i++)
	{		
		cout << list_l_op[i] << " ";
	}
	cout << "\n\n   S-F: ";
	for(int i = 0; i != 4; i++)		
		cout << list_s_op[i] << " ";
	cout << "\n\n o Enter 0 to exit."
			"\n\n o Examples:"
			"\n   1- add x3, x1, x2"
			"\n   2- addi x2, x1, -50"
			"\n   3- ld x2, 8(x1)"
			"\n   4- sd x1, 8(x2)";
	line();
	cout << "\n Enter RISC-V code: ";
	
	while(true)
	{
		getline(cin, input_code);
		if(input_code != "")
			break;
	}
	if(input_code == "0")
		exit_prog = true;
}

void tokenizer()
{
	stringstream s(input_code);
	string temp;
	int i = -1;
	while(s >> temp)
	{
		i++;
		parts[i] = temp;
	}
}

void analyse_parts()
{	
	check_op();
	if(!checked)
		return;
	switch(format_code)
	{
		case 1:
			r_format();
			break;
		case 2:
			i_format();
			break;
		case 3:
			i_format();
			break;
		case 4:
			s_format();
	}
}

void check_op()
{	
	checked = false;
	for(int i = 0; i != 10; i++)
	{
		if(list_r_op[i] == parts[0])
		{
			op_code = i;
			checked = true;
			format_code = 1;
			break;
		}
		if(i > 8)
			continue;
		if(list_i_op[i] == parts[0])
		{
			op_code = i;
			checked = true;
			format_code = 2;
			break;
		}
		if(i > 6)
			continue;
		if(list_l_op[i] == parts[0])
		{
			op_code = i;
			checked = true;
			format_code = 3;
			break;
		}
		if(i > 3)
			continue;
		if(list_s_op[i] == parts[0])
		{
			op_code = i;
			checked = true;
			format_code = 4;
			break;
		}
	}
}

void r_format()
{
	opcode = "0110011";
	funct7 = "0000000";
	
	if(op_code == 8 || op_code == 9)             // Exception for sub and sra.
	{
		funct7 = "0100000";
		if(op_code == 8)
			funct3 = "000";
		else
			funct3 = "101";
	} else
	{
		funct3 = convert_to_binary(op_code);     // Make funct3.
		int len = funct3.length();
		for(int i = 0; i != 3 - len; i++)        // Extend sign.
			funct3 = "0" + funct3;
	}
	
	parts[1].erase(parts[1].size() - 1);         // Erase Commas.
	parts[2].erase(parts[2].size() - 1);
	
	for(int i = 1; i != 4; i++)                  // Make rd, rs1, rs2
	{
		check_register(parts[i]);
		if(!checked)
			return;
		make_register(i, parts[i]);
	}
}

void i_format()
{
	funct3 = convert_to_binary(op_code);         // Make funct3.
	if(op_code == 8)
		funct3 = "101";                          // Exception for srai.
	int len = funct3.length();
	for(int i = 0; i != 3 - len; i++)            // Extend sign.
		funct3 = "0" + funct3;
	
	if(format_code == 2)                         // Not Load operations.
	{
		opcode = "0010011";
	
		parts[1].erase(parts[1].size() - 1);     // Erase Commas.
		parts[2].erase(parts[2].size() - 1);
		for(int i = 1; i != 3; i++)              // Make rd, rs1
		{
			check_register(parts[i]);
			if(!checked)
				return;
			make_register(i, parts[i]);
		}
		
		make_imm_12();
		if(!checked)
			return;
	} else                                       // Load operations.
	{
		opcode = "0000011";
		parts[1].erase(parts[1].size() - 1);     // Erase Comma.
		check_register(parts[1]);
		if(!checked)
			return;
		make_register(1, parts[1]);              // Make rd
		
		int ind = 0;                   
		parts[3] = "";
		while(parts[2][ind] != '(')              // Separates the number.
		{
			parts[3] += parts[2][ind];
			ind++;
			if(ind == parts[2].length())
			{
				checked = false;
				return;
			}
		}
		make_imm_12();
		if(!checked)
			return;
		
		ind++;
		string temp = "";
		while(parts[2][ind] != ')')              // Separates the register.
		{
			temp += parts[2][ind];
			ind++;
			if(ind == parts[2].length())
			{
				checked = false;
				return;
			}
		}
		check_register(temp);
		if(!checked)
			return;
		make_register(2, temp);
	}
	format_code = 2;
}

void s_format()
{
	opcode = "0100011";
	funct3 = convert_to_binary(op_code);         // Make funct3.
	int len = funct3.length();
	for(int i = 0; i != 3 - len; i++)            // Extend sign.
		funct3 = "0" + funct3;
	
	parts[1].erase(parts[1].size() - 1);         // Erase Comma.
	check_register(parts[1]);
	if(!checked)
		return;
	make_register(3, parts[1]);                  // Make rs2
	
	int ind = 0;                   
	parts[3] = "";
	while(parts[2][ind] != '(')                  // Separates the number.
	{
		parts[3] += parts[2][ind];
		ind++;
		if(ind == parts[2].length())
		{
			checked = false;
			return;
		}
	}
	make_imm_12();
	if(!checked)
		return;
	
	imm_p2 = imm_p1 = "";
	for(int i = 0; i != 7; i++)                  // Separates the immediate.
		imm_p2 += imm_12[i];
	for(int i = 7; i != 12; i++)
		imm_p1 += imm_12[i];	
	
	ind++;
	string temp = "";
	while(parts[2][ind] != ')')                  // Separates the register.
	{
		temp += parts[2][ind];
		ind++;
		if(ind == parts[2].length())
		{
			checked = false;
			return;
		}
	}
	check_register(temp);
	if(!checked)
		return;
	make_register(2, temp);                      // Make rs1
}

void make_imm_12()
{
	stringstream a;
	int im;
	a << parts[3];
	a >> im;
	if(a.fail())
	{
		checked = false;
		return;
	}
	string temp = convert_to_binary(im);
	int len = temp.length();
	
	if(op_code == 1 || op_code == 5 || op_code == 8)
	{                                            // Exception for shifts.
		if(im > 6 || im < 0)                     // Check i in shifting.
		{
			checked = false;
			cout << "\n [In shifting: 0 <= i < 6]\n";
			return; 
		}
		
		imm_12 = "000000";
		if(op_code == 8)                         // Exception for srai.
			imm_12 = "010000";
 		for(int i = 0; i != 6 - len; i++)        // Extend sign.
			temp = "0" + temp;
		imm_12 += temp;                          // Make immediate.	
	} else if(im >= 0)
	{
		for(int i = 0; i != 12 - len; i++)       // Extend sign.
			temp = "0" + temp;
		imm_12 = temp;
	} else
	{
		for(int i = 0; i != 12 - len; i++)       // Extend sign.
			temp = "1" + temp;
		imm_12 = temp;
	}  
}

void check_register(string reg)
{
	checked = false;
	for(int i = 0; i != 32; i++)
	{
		if(list_register[i] == reg)
		{
			checked = true;
			break;
		}
	}
}

void make_register(int select, string reg)
{
	reg.erase(0, 1);                             // Erase the x.
	stringstream a;
	int num;
	a << reg;
	a >> num;
	reg = convert_to_binary(num);
	
	int size = reg.length(); 
	for(int i = 0; i != 5 - size; i++)           // Extend sign.
		reg = "0" + reg;
	
	switch(select)
	{
		case 1:
			rd = reg;
			break;
		case 2:
			rs1 = reg;
			break;
		case 3:
			rs2 = reg;
			break;
	}
}

void make_hex_code()
{
	string temp = "";
	string temp2 = "";
	hex_code = "";
	int ind = 0;
	int num;
	for(int i = 0; i != 8; i++)
	{
		for(int j = 0; j != 4; j++)
		{
			temp += binary_code[ind];
			ind++; 
		}
		temp2 += temp;
		num = convert_to_decimal(temp);
		hex_code += convert_to_hex(num);
		temp = "";
		if(i != 7)
			temp2 += " ";
	}
	binary_code = temp2;
}

void print_results()
{
	cout << "\n                    < Result >";
	line();
	cout << "\n Main Code:\t" << input_code <<
			"\n\n opcode:\t" << opcode << 
	    	"\n funct3:\t" << funct3 <<
	    	"\n rs1:\t\t" << rs1;
	switch(format_code)
	{
		case 1:
			cout << "\n rs2:\t\t" << rs2
				 << "\n rd:\t\t" << rd
				 << "\n funct7:\t" << funct7;
			break;
		case 2:
			cout << "\n rd:\t\t" << rd
			     << "\n immediate:\t" << imm_12;
			break;
		case 4:
			cout << "\n rs2:\t\t" << rs2
			     << "\n imm p1:\t" << imm_p1
				 << "\n imm p2:\t" << imm_p2;
	}
	line();
	cout << "\n Binary code:\n " << binary_code <<
			"\n\n Hex code: " << hex_code;
	line();
	cout << "\n press any key to go back";
	_getch(); 
}

string convert_to_binary(int number)
{	
	bool negative = false;
	if(number < 0)
	{
		negative = true;
		number *= -1;
	}
	int sum = 0;
	int index = 0;
	int rem;
	
	while(number != 0)                           
	{
		rem = number % 2;
		number = number / 2;
		sum += rem * pow(10, index);
		index++;
	}
	
	stringstream a;                              
	a << sum;
	string num;
	a >> num;
	
	if(negative)                                 // Makes the binary negative.
	{
		int len = num.length();
		for(int i = 0; i != len; i++)            // Complement
		{
			if(num[i] == '0')
				num[i] = '1';
			else
				num[i] = '0';
		}

		int n = convert_to_decimal(num);
		n++;                                     // Complement Plus 1
		num = convert_to_binary(n);
		int len2 = num.length();
		for(int i = 0; i != len - len2; i++)     // Extend sign.
			num = "0" + num;
		num = "1" + num;                         // Add sign bit.
	}
	return num;
}

int convert_to_decimal(string number)
{
	int num;
	stringstream a;
	a << number;
	a >> num;
	int index, sum, rem;
	index = sum = 0;
	while(num > 0)
	{
		rem = num % 10;
		num /= 10;
		sum += rem * pow(2, index);
		index++;
	}
	
	return sum;
}

string convert_to_hex(int num)
{
	stringstream a;
	string temp;
	if(num <= 9)
	{
		a << num;
		a >> temp;
		return temp;
	}
	
	switch(num)
	{
		case 10:
			return "A";
		case 11:
			return "B";
		case 12:
			return "C";
		case 13:
			return "D";
		case 14:
			return "E";
		case 15:
			return "F";
	}
}

void make_lists()
{
	list_r_op[0] = "add";                        // R-format op
	list_r_op[1] = "sll";
	list_r_op[2] = "slt";
	list_r_op[3] = "sltu";
	list_r_op[4] = "xor";
	list_r_op[5] = "srl";
	list_r_op[6] = "sra";
	list_r_op[7] = "or";
	list_r_op[8] = "sub";
	list_r_op[9] = "sra";

	list_i_op[0] = "addi";                       // I-format op
	list_i_op[1] = "slli"; 
	list_i_op[2] = "slti"; 
	list_i_op[3] = "sltiu"; 
	list_i_op[4] = "xori"; 
	list_i_op[5] = "srli";  
	list_i_op[6] = "ori"; 
	list_i_op[7] = "andi";
	list_i_op[8] = "srai";
	
	list_l_op[0] = "lb";                         // I-format load
	list_l_op[1] = "lh";
	list_l_op[2] = "lw";
	list_l_op[3] = "ld";
	list_l_op[4] = "lbu";
	list_l_op[5] = "lhu";
	list_l_op[6] = "lwu";

	list_s_op[0] = "sb";                         // S-format op
	list_s_op[1] = "sh";
	list_s_op[2] = "sw";
	list_s_op[3] = "sd";
	
	stringstream a;                              // Registers
	string temp;
	for(int i = 0; i != 32; i++)
	{
		a.clear();
		a << i;
		a >> temp;
		list_register[i] = "x" + temp;
	}
}

void line() 
{
	cout << "\n\n --------------------------------------------------\n";
}


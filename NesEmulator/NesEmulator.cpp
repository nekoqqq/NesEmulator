/*
NES模拟器使用C++实现
*/


#include<iostream>
#include<vector>
#include<cstdio>
#include<cassert>
#include<string>
#include<unordered_map>
using namespace std;

const int MEM_SIZE = 0x10000; // 2**16 次方，寻址用2个字节，最多65536字节，也就是64 KB内存


using byte = unsigned char;


// 寻址方式
enum AddressingMode {
	Immediate,
	ZeroPage,
	ZeroPage_X,
	ZeroPage_Y,
	Absolute,
	Absolute_X,
	Absolute_Y,
	Indirect_X,
	Indirect_Y,
	NoneAddressing
};


/*	6502 CPU模拟
* 采用小端方式，也就是说一个内存地址0x8000
* 在大端下是80 00，而在小端下是00 80
* 例如 LDA $8000 <=> ad 00 80
*/


// 每一种类型的操作指令
class OpCode {
public:
	byte code;
	string mnemonic; // todo 如何只做一份string
	int size;
	int cycles;
	AddressingMode mode;
	static vector<OpCode> CPU_OPS_CODES; // 所有指令共享的操作码，其实就是打个表，可以根据操作码查出对应的映射，以及相应的一些属性，如果用python实现，就是一个大map
	static unordered_map<byte, OpCode> OPCODES_MAP; // 所有成员共享的东西，这个当然可以放在外面定义，但是和指令是有关系的，所以放在这里


public:
	OpCode(byte code=0, string mnemonic="", int size=0, int cycles=0, AddressingMode mode=NoneAddressing) :code(code), mnemonic(mnemonic), size(size), cycles(cycles), mode(mode) {

	}
	static unordered_map<byte,OpCode> opcodes_map_init() {
		unordered_map<byte, OpCode> res;
		for (auto cpu_ops_code : CPU_OPS_CODES) {
			res[cpu_ops_code.code]= cpu_ops_code;
		}
		return res;
	}
};
vector<OpCode> OpCode::CPU_OPS_CODES{
	OpCode(0x00,"BRK",1,7,NoneAddressing),
	OpCode(0xea,"NOP",1,2,NoneAddressing),

	// AND
	OpCode(0x29,"AND",2,2,Immediate),
	OpCode(0x25,"AND",2,3,ZeroPage),
	OpCode(0x35,"AND",2,4,ZeroPage_X),
	OpCode(0x2d,"AND",3,4,Absolute),
	OpCode(0x3d,"AND",3,4,Absolute_X), // +1 if page crossed
	OpCode(0x39,"AND",3,4,Absolute_Y), // +1 if page crossed
	OpCode(0x21,"AND",2,6,Indirect_X),
	OpCode(0x31,"AND",2,5,Indirect_Y), // +1 if page crossed

	// EOR
	OpCode(0x49,"EOR",2,2,Immediate),
	OpCode(0x45,"EOR",2,3,ZeroPage),
	OpCode(0x55,"EOR",2,4,ZeroPage_X),
	OpCode(0x4d,"EOR",3,4,Absolute),
	OpCode(0x5d,"EOR",3,4,Absolute_X), // +1 if page crossed
	OpCode(0x59,"EOR",3,4,Absolute_Y), // +1 if page crossed
	OpCode(0x41,"EOR",2,6,Indirect_X),
	OpCode(0x51,"EOR",2,5,Indirect_Y), // +1 if page crossed

	// ORA
	OpCode(0x09,"ORA",2,2,Immediate),
	OpCode(0x05,"ORA",2,3,ZeroPage),
	OpCode(0x15,"ORA",2,4,ZeroPage_X),
	OpCode(0x0d,"ORA",3,4,Absolute),
	OpCode(0x1d,"ORA",3,4,Absolute_X), // +1 if page crossed
	OpCode(0x19,"ORA",3,4,Absolute_Y), // +1 if page crossed
	OpCode(0x01,"ORA",2,6,Indirect_X),
	OpCode(0x11,"ORA",2,5,Indirect_Y), // +1 if page crossed

	// ASL
	OpCode(0x0a,"ASL",1,2,NoneAddressing),
	OpCode(0x06,"ASL",2,5,ZeroPage),
	OpCode(0x16,"ASL",2,6,ZeroPage_X),
	OpCode(0x0e,"ASL",3,6,Absolute),
	OpCode(0x1e,"ASL",3,7,Absolute_X),

	// ASR
	OpCode(0x4a,"ASR",1,2,NoneAddressing),
	OpCode(0x46,"ASR",2,5,ZeroPage),
	OpCode(0x56,"ASR",2,6,ZeroPage_X),
	OpCode(0x4e,"ASR",3,6,Absolute),
	OpCode(0x5e,"ASR",3,7,Absolute_X),

	// TAX
	OpCode(0xaa,"TAX",1,2,NoneAddressing),

	// INX
	OpCode(0xe8,"INX",1,2,NoneAddressing),

	// LDA
	OpCode(0xa9,"LDA",2,2,Immediate),
	OpCode(0xa5,"LDA",2,3,ZeroPage),
	OpCode(0xb5,"LDA",2,4,ZeroPage_X),
	OpCode(0xad,"LDA",3,4,Absolute),
	OpCode(0xbd,"LDA",3,4,Absolute_X), // +1 if page crossed
	OpCode(0xb9,"LDA",3,4,Absolute_Y), // +1 if page crossed
	OpCode(0xa1,"LDA",2,6,Indirect_X),
	OpCode(0xb1,"LDA",2,5,Indirect_Y), // +1 if page crossed

	// STA
	OpCode(0x85,"STA",2,3,ZeroPage),
	OpCode(0x95,"STA",2,4,ZeroPage_X),
	OpCode(0x8d,"STA",3,4,Absolute),
	OpCode(0x9d,"STA",3,5,Absolute_X),
	OpCode(0x99,"STA",3,5,Absolute_Y),
	OpCode(0x81,"STA",2,6,Indirect_X),
	OpCode(0x91,"STA",2,6,Indirect_Y),

	// ADC, add with carray
	OpCode(0x69,"ADC",2,2,Immediate),
	OpCode(0x65,"ADC",2,3,ZeroPage),
	OpCode(0x75,"ADC",2,4,ZeroPage_X),
	OpCode(0x6d,"ADC",3,4,Absolute),
	OpCode(0x7d,"ADC",3,4,Absolute_X), // +1 if page crossed
	OpCode(0x79,"ADC",3,4,Absolute_Y), // +1 if page crossed
	OpCode(0x61,"ADC",2,6,Indirect_X),
	OpCode(0x71,"ADC",2,5,Indirect_Y), // +1 if page crossed
	
	// SBC, add with not carry
	OpCode(0xe9,"SBC",2,2,Immediate),
	OpCode(0xe5,"SBC",2,3,ZeroPage),
	OpCode(0xf5,"SBC",2,4,ZeroPage_X),
	OpCode(0xed,"SBC",3,4,Absolute),
	OpCode(0xfd,"SBC",3,4,Absolute_X), // +1 if page crossed
	OpCode(0xf9,"SBC",3,4,Absolute_Y), // +1 if page crossed
	OpCode(0xe1,"SBC",2,6,Indirect_X),
	OpCode(0xf1,"SBC",2,5,Indirect_Y), // +1 if page crossed

	// BCC
	OpCode(0x90,"BCC",2,2,NoneAddressing), // +1 if branch succeeds	+2 if to a new page

	// BCS
	OpCode(0xB0,"BCS",2,2,NoneAddressing), // +1 if branch succeeds +2 if to a new 

	// BEQ
	OpCode(0x90, "BEQ", 2, 2, NoneAddressing), // +1 if branch succeeds	+2 if to a new page

	// BEQ
	OpCode(0xD0, "BNE", 2, 2, NoneAddressing), // +1 if branch succeeds	+2 if to a new page

	// BPL, Branch On Plus
	OpCode(0xD0,"BPL",2,2,NoneAddressing), // +1 if branch succeeds +2 if to a new page



};
unordered_map<byte,OpCode> OpCode::OPCODES_MAP= OpCode::opcodes_map_init();

class CPU {
public:
	// 程序计数器
	uint16_t program_counter;
	byte stack_pointer;

	// 通用寄存器
	byte register_a;
	byte register_x;
	byte register_y;


	// 程序状态字寄存器, N V _ B D I Z C
	//				   7 6 5 4 3 2 1 0
	byte status;

	// 内存 address space,虚拟地址空间,CPU只有2KB的RAM，其他的都是用来做内存映射
	byte memory[MEM_SIZE]; 

public:
	CPU(byte programe_counter = 0, byte stack_pointer = 0, byte register_a = 0, byte register_x = 0, byte register_y = 0, byte status = 0) :program_counter(programe_counter), stack_pointer(stack_pointer), register_a(register_a), register_x(register_x), register_y(register_y), status(status) {}


	void interpret(vector<byte>& program) {
		while (true)
		{
			// 取指
			byte opscode = program[program_counter++];

			switch (opscode)
			{

			// LDA Immediate
			case 0xA9: lda(program); break;

			// TAX Immediate
			case 0xAA: tax(program); break;

			case 0xE8:inx(program); break;


			case 0xC8:
				iny(program);
				break;

			// BRK 
			case 0x00: {brk(program); return; break; }

			default:
				break;
			}

		}

	}

	byte mem_read(uint16_t addr) {
		return memory[addr];
	}

	void mem_write(uint16_t addr, byte data) {
		memory[addr] = data;
	}


	// 将ROM加载到内存中
	void load(vector<byte>& program) {
		// 从32 KB处开始加载程序,程序计数器也设置为此,ROM加载到此内存里面，并不代表实际就从0x8000开始执行了
		memcpy(memory+0x8000,&program[0], program.size() * sizeof(byte));

		// 在NES插入卡带后，会重置程序状态，并将程序计数器的位置设置为存储在0xFFFC处的值,[X,X,X,X] 也就是最后4个字节
		// 将0xFFFC处存储的值设置为0x8000;
		mem_write_u16(0xFFFC, 0x8000);
	}

	// 加载ROM并运行程序
	void load_and_run(vector<byte>& program) {
		load(program);
		reset();
		run();

	}

	// 给一个16位的地址，需要读取连续两个字节的数据，也就是连续两行的数据，这两行的数据再根据小端的方式拼接
	uint16_t mem_read_u16(uint16_t pos) {
		uint16_t lo = mem_read(pos);
		uint16_t hi = mem_read(pos + 1);
		return hi << 8 | lo;
	}

	// 给一个16位的地址，写入一个16位的操作数，用小端的方式，先写低八位的数据，再写高8位的数据
	// 记住，内存总共64KB大小，总共有64K行，每行都是8位，存一个Byte，数据就存在这些cell里面
	void mem_write_u16(uint16_t pos, uint16_t data) {
		byte hi = (data >> 8); // 高8位
		byte lo = (data & 0xff); // 低8位
		mem_write(pos, lo);
		mem_write(pos + 1, hi);
	}

	// 重置寄存器的状态，将program_counter的值设置为在内存0xFFFC处存储的2个字节的值
	void reset() {
		register_a = 0;
		register_x = 0;
		status = 0;

		program_counter = mem_read_u16(0xFFFC);
	}

	void run() {
		while (true)
		{
			// 取指
			byte code = mem_read(program_counter++);
			uint16_t program_counter_state = program_counter;

			OpCode op_code = OpCode::OPCODES_MAP.at(code);


			vector<byte> program;
			switch (code)
			{
				// LDA
				case 0xa9:
				case 0xa5:
				case 0xb5:
				case 0xad:
				case 0xbd:
				case 0xb9:
				case 0xa1:
				case 0xb1:lda(op_code.mode); break;

				// STA
				case 0x85:
				case 0x95:
				case 0x8d:
				case 0x9d:
				case 0x99:
				case 0x81:
				case 0x91:sta(op_code.mode); break;


				// ADC
				case 0x69:
				case 0x65:
				case 0x75:
				case 0x6d:
				case 0x7d:
				case 0x79:
				case 0x61:
				case 0x71:adc(op_code.mode); break;
				
				// SBC
				case 0xe9:
				case 0xe5:
				case 0xf5:
				case 0xed:
				case 0xfd:
				case 0xf9:
				case 0xe1:
				case 0xf1:sbc(op_code.mode); break;

				// AND
				case 0x29:
				case 0x25:
				case 0x35:
				case 0x2d:
				case 0x3d:
				case 0x39:
				case 0x21:
				case 0x31:op_and(op_code.mode); break;

				// EOR
				case 0x49:
				case 0x45:
				case 0x55:
				case 0x4d:
				case 0x5d:
				case 0x59:
				case 0x41:
				case 0x51:op_eor(op_code.mode); break;

				// ORA
				case 0x09:
				case 0x05:
				case 0x15:
				case 0x0d:
				case 0x1d:
				case 0x19:
				case 0x01:
				case 0x11:op_ora(op_code.mode); break;

				// ASL
				case 0x0A:
				case 0x06:
				case 0x16:
				case 0x0e:
				case 0x1e:asl(op_code.mode); break;


				// Branch Family
				// BPL
				case 0x10:bpl(op_code.mode); break;
				// BMI
				case 0x30:bmi(op_code.mode); break;
				// BVC
				case 0x50:bvc(op_code.mode); break;
				// BVS
				case 0x70:bvc(op_code.mode); break;
				// BCC
				case 0x90:bcc(op_code.mode); break;
				// BCS
				case 0xB0:bcs(op_code.mode); break;
				// BNE
				case 0xD0:bne(op_code.mode); break;
				// BEQ
				case 0xF0:beq(op_code.mode); break;


				// TAX
				case 0xaa:tax(program); break;

				// INX
				case 0xe8:inx(program); break;

				// INY
				case 0xC8:iny(program);break;

				// BRK 
				case 0x00: {brk(program); return; break; }


				default:
					break;
				}
			if (program_counter_state == program_counter) {
				program_counter += op_code.size - 1;
			}
		}
	}
protected:
	// 输出的处理函数
	friend ostream& operator<< (ostream& out, CPU& cpu) {
		printf("current cpu state: programe counter: %d, stack pointer: %d, register A: %d, register X: %d, register Y: %d, status: %d", cpu.program_counter, cpu.stack_pointer, cpu.register_a, cpu.register_x, cpu.register_y, cpu.status);
		return out;
	}
	friend ostream& operator<<(ostream& out, CPU* cpu) {
		printf("current CPU state: programe counter: %#X, stack pointer: %#X, register A: %d, register X: %d, register Y: %d\n", cpu->program_counter, cpu->stack_pointer, cpu->register_a, cpu->register_x, cpu->register_y);
		int n = 8;
		int x = cpu->status;
		vector<int> res;
		while (n-- > 0) {
			res.push_back(x & 0x01);
			x >>= 1;
		}
		cout << "CPU status: ";

		unordered_map<int, char> status_ch_map{
			{0, 'C'},
			{ 1,'Z' },
			{ 2,'I' },
			{ 3,'D' },
			{ 4,'B' },
			{ 5,' ' },
			{ 6,'V' },
			{ 7,'N' }
		};
		for (int i = res.size() - 1; i >= 0; i--) {
			cout << status_ch_map[i]<<":"<< res[i] << " ";
		}

		return out;
	}

	bool brk(vector<byte> &program) {
		cout << "halt" << endl; 
		return true;
	}

	bool lda(vector<byte>& program) {
		register_a = program[program_counter++];
		update_zero_and_negative_flags(register_a);

		return true;
	}

	bool lda(AddressingMode mode) {
		uint16_t addr = get_operand_address(mode);
		byte value = mem_read(addr);

		register_a = value;
		update_zero_and_negative_flags(register_a);
		return true;
	}

	bool sta(AddressingMode mode) {
		uint16_t addr = get_operand_address(mode);
		mem_write(addr, register_a);
		return true;
	}

	bool tax(vector<byte>& program) {
		register_x = register_a;
		update_zero_and_negative_flags(register_x);

		return true;
	}

	bool inx(vector<byte>& program)
	{
		register_x += 1;
		update_zero_and_negative_flags(register_x);

		return true;
	}

	bool iny(vector<byte>& program)
	{
		register_y += 1;
		update_zero_and_negative_flags(register_y);

		return true;
	}

	bool adc(AddressingMode mode) {
		// adc(y) 定义为x+y+c
		uint16_t address = get_operand_address(mode);
		byte value = mem_read(address);

		add_to_register_a(value);
		
		return true;
	}


	bool sbc(AddressingMode mode) {
		uint16_t address = get_operand_address(mode);
		byte value = mem_read(address);
		// sbc(y) 定义为x-y-(1-c)
		// 推导 x-y-(1-c) = x-y-1+c = x+(~y+1)-1+c = x+~y+c=add(~y)
		// -y-1的操作需要注意，对于-128 这个特殊情况
		// 如果保存在unsigned char中，有一个转换，转换为128，则机器数为1000_0000,取了负数的操作，仍然是1000_0000(取负操作是各位取反，末位+1，则变为128
		// 如果保存在signed char中，机器数仍然是1000_0000，取了负数的操作，仍然是1000_0000(取负操作是各位取反，末位+1），解释仍然为-128
		// 取负操作的时候，需要注意，计算机都是各位取反，末位+1
		// 所以其实在函数传递参数的时候，传-y-1传的是一个字面量，取负操作，需要把y当作有符号数据对待？如果不进行转换呢？
		add_to_register_a(~value);
		return true;

	}

	bool op_and(AddressingMode mode) {
		uint16_t addr = get_operand_address(mode);
		byte data = mem_read(addr);
		register_a = data & register_a;
		update_zero_and_negative_flags(register_a);
		return true;
	}

	bool op_eor(AddressingMode mode) {
		uint16_t addr = get_operand_address(mode);
		byte data = mem_read(addr);
		register_a = data ^ register_a;
		update_zero_and_negative_flags(register_a);
		return true;
	}

	bool op_ora(AddressingMode mode) {
		uint16_t addr = get_operand_address(mode);
		byte data = mem_read(addr);
		register_a = data | register_a;
		update_zero_and_negative_flags(register_a);
		return true;
	}

	bool asl(AddressingMode mode) {
		uint16_t addr = get_operand_address(mode);
		byte data = mem_read(addr);

		int carry = data | 0x80;

		if (data >> 7 == 1) {
			status = status | 0x01;
		}
		else {
			status = status & 0xfe;
		}

		data = data << 1;
		mem_write(addr, data);
		update_zero_and_negative_flags(data);
		return true;
	}

	bool asr(AddressingMode mode) {
		uint16_t addr = get_operand_address(mode);
		byte data = mem_read(addr);

		int carry = data | 0x80;

		if (data >> 7 == 1) {
			status = status | 0x01;
		}
		else {
			status = status & 0xfe;
		}

		data = data << 1;
		mem_write(addr, data);
		update_zero_and_negative_flags(data);
		return true;
	}

	bool bcc(AddressingMode mode) {
		// if carry clear
		if ((status & 1<<0) == 0) {
			int8_t  jump = mem_read(program_counter);
			uint16_t jump_addr = program_counter + 1 + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
			program_counter = jump_addr;
		}
		return true;
	}

	bool bcs(AddressingMode mode) {
		// if carry clear
		if ((status & 1<<0) == 1) {
			int8_t  jump = mem_read(program_counter);
			uint16_t jump_addr = program_counter + 1 + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
			program_counter = jump_addr;
		}
		return true;
	}

	// Branch On Equal
	bool beq(AddressingMode) {
		// if zero clear
		if ((status & 1<<1) == 0) {
			int8_t  jump = mem_read(program_counter);
			uint16_t jump_addr = program_counter + 1 + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
			program_counter = jump_addr;
		}
		return true;
	}

	// Branch On Not Equal
	bool bne(AddressingMode) {
		// if zero set
		if ((status & 1<<1) == 1) {
			int8_t  jump = mem_read(program_counter);
			uint16_t jump_addr = program_counter + 1 + jump; // +1是因为要从下个指令的地址开始，加上这个跳转的数值
			program_counter = jump_addr;
		}
		return true;
	}

	bool bpl(AddressingMode) {
		if ((status & 1 << 7) == 0){
			int8_t jump = mem_read(program_counter);
			uint16_t jump_addr = program_counter + 1 + jump;
			program_counter = jump_addr;
		}
		return true;
	}

	bool bmi(AddressingMode) {
		if ((status & 1 << 7) == 1) {
			int8_t jump = mem_read(program_counter);
			uint16_t jump_addr = program_counter + 1 + jump;
			program_counter = jump_addr;
		}
		return true;
	}

	bool bvc(AddressingMode) {
		if ((status & 1 << 6) == 0) {
			int8_t jump = mem_read(program_counter);
			uint16_t jump_addr = program_counter + 1 + jump;
			program_counter = jump_addr;
		}
		return true;
	}

	bool bvs(AddressingMode) {
		if ((status & 1 << 6) == 1) {
			int8_t jump = mem_read(program_counter);
			uint16_t jump_addr = program_counter + 1 + jump;
			program_counter = jump_addr;
		}
		return true;
	}

	// 通用的设置寄存器的函数
	void update_zero_and_negative_flags(byte res) {
		if (res == 0) {
			status = status | 0x02; // 0000_0010
		}
		else {
			status = status & 0xfd; // 1111_1101
		}

		// set flag N,置成1就用|，置成0就用&
		if (res & 0x80) { // 10000000
			status = status | 0x80; // 1000_0000 
		}
		else {
			status = status & 0x7f; // 0111_1111
		}
	}


	uint16_t get_operand_address(AddressingMode& mode) {
		uint16_t address;
		switch (mode)
		{
			// 立即数，当前内存地址为PC指向
		case Immediate:
			address = program_counter;
			break;

			// 只有一个byte，可以索引0-255的地址
		case ZeroPage:
			address = mem_read(program_counter);
			break;

			// 两个字节，索引0-65535的整个地址空间
		case Absolute:
			address=mem_read_u16(program_counter);
			break;

			// 用X里面存储的值+一个byte的数据做索引
		case ZeroPage_X: {
			byte pos = mem_read(program_counter);
			address = pos + register_x;
		}break;

			// 用Y里面存储的值+一个byte的数据做索引
		case ZeroPage_Y: {
			byte pos = mem_read(program_counter);
			address = pos + (int)register_y; // 指针偏移
		}break;

		case Absolute_X: {
			uint16_t base = mem_read_u16(program_counter);
			address = base + (int)register_x; // 指针偏移
		}break;

		case Absolute_Y: {
			uint16_t base = mem_read_u16(program_counter);
			address = base + (int)register_y; // 指针偏移
		}break;


			// 先加上X寄存器的值，得到一个地址，再取地址里面保存的两个值，按照小端存储的方式，构成新的地址
		case Indirect_X: {
			byte base = mem_read(program_counter);

			byte ptr = base + (int)register_x;
			byte lo = mem_read(ptr);
			byte hi = mem_read(ptr + 1);
			address = hi << 8 | lo;
		}break;

		case Indirect_Y: {
			byte base = mem_read(program_counter);

			byte ptr = base + (int)register_y;
			byte lo = mem_read(ptr);
			byte hi = mem_read(ptr + 1);
			address = hi << 8 | lo;
		}break;


		case NoneAddressing: {
			printf("not supported mode: %d", mode);
			throw "not supported mode";
		}break;

		default:
			break;
		}

	}

	void add_to_register_a(byte value) {

		int s = register_a + (status & 0x01) + value;

		// 三种判断方式
		// 1. 判断两个符号位相同并且和结果的符号位相同则溢出
		// 2. c6^c7 来判断是否溢出
		// 2. 判断结果是不是比其中的一个数大，因为溢出的时候一定满足result < X AND result < y 

		// carry flag
		if (s > 0xff) {
			status = status | 0x01;
		}
		else {
			status = status & 0xfe;
		}

		byte res = s;

		// overflow flag
		if ((res ^ value) & (res ^ register_a) & 0x80) {
			status = status | 0x40;
		}
		else {
			status = status & 0xBF;
		}

		register_a = res;
		update_zero_and_negative_flags(res);
	}

};

int main() {

	int test_cnt = 1;
	// Test 1 LDA 立即数
	{

		CPU* cpu = new CPU();
		OpCode *opcode = new OpCode();
		cout << cpu << endl;
		vector<byte> test{ 0xa9,0x05,0x00 };
		for (auto x : opcode->OPCODES_MAP) {
			cout << (int)x.first << ":" << x.second.mnemonic << endl;
		}
		cpu->interpret(test);
		assert(cpu->register_a == 0x05);
		assert((cpu->status & 0x02) == 0); // &运算符优先级比==更加的低
		assert((cpu->status & 0x80) == 0);
		cout << cpu << endl;
		cout << "Test passed: " << test_cnt++ << endl;
	}

	// Test 2 LDA 立即数
	{
		CPU* cpu = new CPU();
		cout << cpu << endl;
		vector<byte> test{ 0xa9,0x00,0x00 };
		cpu->interpret(test);
		assert((cpu->status & 0x02) == 0x02);
		cout << cpu << endl;
		cout << "Test passed: " << test_cnt++ << endl;
	}

	// Test 3 TAX 一个字节，包含了一个隐含的操作数
	{
		CPU* cpu = new CPU();
		cout << cpu << endl;
		cpu->register_a = 10;
		vector<byte> test{ 0xaa,0x00 };
		cpu->interpret(test);
		assert(cpu->register_x == 10);
		cout << cpu << endl;
		cout << "Test passed: " << test_cnt++ << endl;
	}

	// Test 4 5个操作一起
	{
		CPU* cpu = new CPU();
		cout << cpu << endl;
		vector<byte> test{ 0xa9,0xc0,0xaa,0xe8,0x00 };
		cpu->interpret(test);
		assert(cpu->register_x == 0xc1);
		cout << cpu << endl;
		cout << "Test passed: " << test_cnt++ << endl;
	}

	// Test 5 INX overflow
	{
		CPU* cpu = new CPU();
		cout << cpu << endl;
		cpu->register_x = 0xff;
		vector<byte> test{ 0xe8,0xe8,0x00 };
		cpu->interpret(test);
		assert(cpu->register_x == 0x01);
		cout << cpu << endl;
		cout << "Test passed: " << test_cnt++ << endl;
	}

	// Test 6 INY overflow
	{
		CPU* cpu = new CPU();
		cout << cpu << endl;
		cpu->register_y = 0xff;
		vector<byte> test{ 0xc8,0xc8,0x00 };
		cpu->interpret(test);
		assert(cpu->register_y == 0x01);
		cout << cpu << endl;
		cout << "Test passed: " << test_cnt++ << endl;
	}

	// Test 7 LDA
	{
		CPU* cpu = new CPU();
		cout << cpu << endl;
		cpu->mem_write(0x10, 0x55);
		vector<byte> program{ 0xa5,0x10,0x00 };
		cpu->load_and_run(program);
		cout << cpu << endl;
		assert(cpu->register_a = 0x55);
		cout << "Test passed: " << test_cnt++ << endl;
	}

	// Test 8 
	{


	}

	{


	}
}
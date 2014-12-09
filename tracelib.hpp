#include<string>
#include<vector>
#include<list>
#include<unordered_set>

using namespace std;

//Types in Elf64
typedef unsigned long Elf64_Addr;
typedef unsigned long Elf64_Off;
typedef unsigned short Elf64_Half;
typedef unsigned int Elf64_Word;
typedef signed int Elf64_Sword;
typedef unsigned long Elf64_Xword;
typedef signed long Elf64_Sxword;

//SYMBOL TABLE ENTRY
typedef struct symentry{
	string name;
	unsigned char type;
	unsigned char bind;
	Elf64_Half st_shndx;
	Elf64_Addr st_value;
} sym_entry;

#define NORMAL_SYM		0
#define DYNAMIC_SYM		1
#define	NO_SYM			2
//LIBRARY INFORMATION
class lib_info{
public:
	int sym_type;
	bool is_used;
	sym_entry* symtab;
	Elf64_Xword symtabsize;
	char* lib_asm;
	unordered_map<string,sym_entry *> table;
	vector<string> dependency;
	lib_info(){
		is_used = false;
	}
	lib_info(sym_entry* symtab,Elf64_Xword symtabsize,char* lib_asm,int sym_type){
		this->symtab = symtab;
		this->symtabsize = symtabsize;
		this->lib_asm = lib_asm;
		this->sym_type = sym_type;
		this->is_used = false;
	}
};

//DATA STRUCTURE FOR GRAPH
class Node{
public:
	string func;
	string lib;
	unsigned char bind;
	const char * func_asm;
	unordered_set<Node*> descendance;
	unsigned long start_offset;
	Node(string func,string lib,unsigned char bind,char* func_asm=NULL){
		this->func = func;
		this->lib = lib;
		this->bind = bind;
		this->func_asm = func_asm;
	}
	void push(Node * node){
		descendance.insert(node);
	}
	string print(){
		return func+"@"+lib;
	}
};

class Graph{
public:
	unordered_map<string,int> libs;
	unordered_map<string,Node *> table;
	Node* find_node(string &func,string &lib){
		string key(func+'@'+lib);
		if( table.find(key) == table.end() ){
			return NULL;
		}else{
			return table[key];
		}
	}
	Node* addnode(string func,string lib,unsigned char bind,unordered_map<string,lib_info> &lib_info_map){
		Node* node = new Node(func,lib,bind);
		string key(func+'@'+lib);
		table[key] = node;
		return node;
	}
	bool removenode(string &func,string &lib){
		if( find_node(func,lib) ){
			string key(func+'@'+lib);
			table.erase(key);
			return true;
		}else
			return false;
	}
};

//SECTION INDEX
#define SHN_UNDEF	0x0000
#define	SHN_LOPROC	0xff00
#define SHN_HIPROC	0xff1f
#define SHN_LOOS	0xff20
#define SHN_HIOS	0xff3f
#define SHN_ABS		0xfff1 
#define	SHN_COMMON	0xfff2

//BINDINGS IN SYMBOL TABLE
#define STB_LOCAL 	0
#define STB_GLOBAL 	1
#define STB_WEAK 	2
#define STB_LOOS	10
#define STB_HIOS	12
#define STB_LOPROC	13
#define STB_HIPROC	15

//TYPES IN SYMBOL TABLE
#define STT_NOTYPE	0
#define STT_OBJECT	1
#define STT_FUNC	2
#define STT_SECTION	3
#define STT_FILE	4
#define STT_LOOS	10
#define STT_HIOS	12
#define STT_LOPROC	13
#define STT_HIPROC	15

//DYNAMIC TABLE TYPE
#define	DT_NULL		0
#define DT_NEEDED	1

//OFFSETS INTO ELF HEADER
#define	E_SHOFF_OFFSET		sizeof(unsigned char)*16+sizeof(Elf64_Half)*2+sizeof(Elf64_Word)+sizeof(Elf64_Addr)+sizeof(Elf64_Off) //40
#define E_SHENTSIZE_OFFSET	sizeof(unsigned char)*16+sizeof(Elf64_Half)*5+sizeof(Elf64_Word)*2+sizeof(Elf64_Addr)+sizeof(Elf64_Off)*2 //58

//OFFSETS INTO SECTION HEADER
#define SH_NAME_OFFSET		0
#define SH_TYPE_OFFSET		sizeof(Elf64_Word)  
#define SH_FLAGS_OFFSET		sizeof(Elf64_Word)*2
#define SH_ADDR_OFFSET		sizeof(Elf64_Word)*2+sizeof(Elf64_Xword)
#define SH_OFFSET_OFFSET 	sizeof(Elf64_Word)*2+sizeof(Elf64_Xword)+sizeof(Elf64_Addr)
#define SH_SIZE_OFFSET 		sizeof(Elf64_Word)*2+sizeof(Elf64_Xword)+sizeof(Elf64_Addr)+sizeof(Elf64_Off)
#define SH_LINK_OFFSET		sizeof(Elf64_Word)*2+sizeof(Elf64_Xword)*2+sizeof(Elf64_Addr)+sizeof(Elf64_Off)
#define	SH_INFO_OFFSET		sizeof(Elf64_Word)*3+sizeof(Elf64_Xword)*2+sizeof(Elf64_Addr)+sizeof(Elf64_Off)
#define	SH_ADDRALIGN_OFFSET	sizeof(Elf64_Word)*4+sizeof(Elf64_Xword)*2+sizeof(Elf64_Addr)+sizeof(Elf64_Off)
#define	SH_ENTSIZE_OFFSET	sizeof(Elf64_Word)*4+sizeof(Elf64_Xword)*3+sizeof(Elf64_Addr)+sizeof(Elf64_Off)

//INITIALIZATION FUNCTIONS
void set_path_ld_verbose();
void set_path_ld_config(string &filename);

//RETURN VALUE OF getdynsym()
#define GETDYNSYM_SUCCESS 	0
#define GETDYNSYM_NOFILE  	1
#define GETDYNSYM_NODYNSYM	2
int getdynsym(string,sym_entry* &,Elf64_Xword &);

//RETURN VALUE OF getsym()
#define GETSYM_SUCCESS	 	0
#define GETSYM_NOFILE 	 	1
#define GETSYM_NODBG	 	2
#define GETSYM_NODBGSYM	 	3
int	getsym(string,sym_entry* &,Elf64_Xword &);

char* readasm(string &);

//RETURN VALUE OF loadlib()
#define LOADLIB_SUCCESS	0
#define LOADLIB_FAIL	1
#define LOADLIB_EXIST	2
int loadlib(string);

//RETURN dependency of file in .dynamic sectoin
vector<string> getdependlib(string &filename);

//SET lib_path to complete path base on default_path
bool find_lib(string lib_name,string &lib_path);

//RETURN library contain function
string search_load_lib(vector<string> &dependency,string &function,unsigned char bind);


vector<string> traverse(string &,string &,unordered_map<string,sym_entry*> &table);


#define ST_VALUE_STR_SIZE 50
char* extract_asm(const char* objdump_buffer,sym_entry* sym);

//FOR DEBUG USE ONLY
void display_strtab(char* &,Elf64_Xword &,const char*);
void display_symtab(lib_info &,const char*);
void display_dependency(lib_info &,const char*);
void show_trace(string,vector<string> &);

//bool getdebuglink(ifstream &,string &,char* shstr);
//void getshstrtab(ifstream &,char* &,Elf64_Xword &);


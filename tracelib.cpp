#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<iomanip>
#include<string>
#include<vector>
#include<cstring>
#include<unordered_map>
#include<unordered_set>
#include<queue>
#include"tracelib.hpp"

using namespace std;

vector<string> default_path;
const char* debug_dir = "/usr/lib/debug";

unordered_map<string,lib_info> lib_info_map;
unordered_map<string,char*> func_path_map;
Graph graph;

const char * help_message = "\
Usage:\n\
1:	./tracelib -v					#printf usage\n\
2:	./tracelib filename -f				#printf functions used\n\
3:	./tracelib filename -t				#printf functions trace\n\
4:	./tracelib filename -a				#printf functions assembly\n\
";
								

void test(){
	string filename("test");
	string libname1("libc.so.6");
	string libname2("libpthread.so.0");
	loadlib(libname1);
	loadlib(libname2);
	//display_dependency(lib_info_map["libc.so.6"],"libc.so.6:");
	//display_dependency(lib_info_map["libpthread.so.0"],"libpthread.so.0:");
	//display_symtab(lib_info_map["libc.so.6"].symtab,lib_info_map["libc.so.6"].symtabsize,"libc.so.6 symbol table");
}

int main(int argc,char *argv[]){
	if( argc == 1 || (argc == 2 && argv[1] == "-v") || argc != 3 ){
		cerr << help_message << endl;
		return 0;
	}
	char flag = argv[2][1];

	setdefaultpath();
	string filename(argv[1]);

	sym_entry* dynsym;
	Elf64_Xword dynsymsize;
	vector<string> function;
	vector<string> funclib;
	vector<unsigned char> binds;
	vector<string> dependency;
	dependency = getdependlib(filename);

	getdynsym(filename,dynsym,dynsymsize);
	for( int i = 0 ; i < dynsymsize ; i++ ){
		if( dynsym[i].type == STT_FUNC && dynsym[i].st_shndx == SHN_UNDEF ){
			string flib = search_load_lib(dependency,dynsym[i].name,dynsym[i].bind);
			if( flib != "" ){
				function.push_back(dynsym[i].name);
				funclib.push_back(flib);
				binds.push_back(dynsym[i].bind);
			}
		}
	}
	//display_symtab(lib_info_map["libc.so.6"],"");
	queue<Node *> nodeq;
	for( int i = 0 ; i < function.size() ; i++ ){
		Node * node = graph.addnode(function[i],funclib[i],binds[i]);
		nodeq.push(node);
	}
	while( !nodeq.empty() ){
		Node *node = nodeq.front();
		nodeq.pop();
		unordered_map<string,sym_entry*> &table = lib_info_map[node->lib].table;
		vector<string> &dependency = lib_info_map[node->lib].dependency;

		vector<string> out_funcs;
		out_funcs = traverse(node->lib,node->func,table);
		for( int i = 0 ; i < out_funcs.size() ; i++ ){
			//cout << node->func << "@" << node->lib << " -> " << out_funcs[i] << endl;
			if( table.find(out_funcs[i]) == table.end() ){
				cerr << "ERROR 1: " << out_funcs[i] << " symbol not found in " << node->lib << endl;
				exit(-1);
			}else{
				//if out_funcs[i] has definition in current lib
				if( table[out_funcs[i]]->st_shndx == SHN_UNDEF ){
					string lib_path;
					find_lib(node->lib,lib_path);
					vector<string> new_dependency = getdependlib(lib_path);
					string new_lib = search_load_lib(new_dependency,out_funcs[i],table[out_funcs[i]]->bind);
					if( new_lib == "" )
						continue;
					if( graph.find(out_funcs[i],new_lib) == NULL )
						nodeq.push(graph.addnode(out_funcs[i],new_lib,lib_info_map[new_lib].table[out_funcs[i]]->bind));
				}else{
					if( graph.find(out_funcs[i],node->lib) == NULL ){
						nodeq.push(graph.addnode(out_funcs[i],node->lib,table[out_funcs[i]]->bind));
					}
				}
			}
			node->descendance.insert(nodeq.back());
		}
	}
	for( int i = 0 ; i < graph.nodes.size() ; i++ ){
		cout << graph.nodes[i]->func << "@" << graph.nodes[i]->lib << endl;
		if( flag == 'a' )
			cout << graph.nodes[i]->func_asm << endl;
		if( flag == 't' ){
			for( auto j = graph.nodes[i]->descendance.begin() ; j != graph.nodes[i]->descendance.end() ; j++ ){
				cout << "---->" << (*j)->func << "@" << (*j)->lib << endl;
			} 
			cout << endl;
		}
	}
	//test();

	return 0;
}


/*******************************************************************************
	Search function in dependency shared library
	Load library if library is not load before
*******************************************************************************/
string search_load_lib(vector<string> &dependency,string &function,unsigned char bind){
	string lib;
	for( int i = 0 ; i < dependency.size() ; i++ ){
		if( lib_info_map.find(dependency[i]) == lib_info_map.end() )
			loadlib(dependency[i]);
		unordered_map<string,sym_entry*> &table = lib_info_map[dependency[i]].table;
		if( table.find(function) != table.end() && table[function]->bind != STB_LOCAL && table[function]->st_shndx != SHN_UNDEF && bind <= table[function]->bind ){
			//bind <= table[function]->bind means bind is weaker than table[function]->bind 
			lib = dependency[i];
			return lib;
		}
	}
	return lib;
}

/*******************************************************************************
	Set default search path
*******************************************************************************/
void setdefaultpath(){
	system(string("ld --verbose | grep \"SEARCH_DIR(\\\"\" > default_path.txt").c_str());
	ifstream infile("default_path.txt");
	infile.seekg(0,ifstream::end);
	unsigned long length = infile.tellg();
	length++;
	infile.seekg(0,ifstream::beg);
	char * buffer = new char[length];
	infile.read((char *)buffer,length-1);
	buffer[length-1]=0;
	char *spos = buffer,*epos;
	while( (spos = strstr(spos,"SEARCH_DIR(\"")) != NULL ){
		spos += strlen("SEARCH_DIR(\"");
		if( *spos == '=' ) spos++;
		epos = strstr(spos,"\")");
		string path(spos,epos-spos);
		default_path.push_back(path);
		spos = epos;
	}
	delete[] buffer;
}

char* extract_asm(const char* objdump_buffer,sym_entry* sym){
	char * func_asm;
	char st_value_str[ST_VALUE_STR_SIZE];
	sprintf(st_value_str,"%016lx",sym->st_value);
	strcat(st_value_str," <");
	const char *pos_start,*pos_end;
	pos_start = strstr(objdump_buffer,st_value_str);
	pos_end = strstr(pos_start,"\n\n");

	unsigned long length = pos_end - pos_start + 2;
	func_asm = new char[length];
	
	unsigned long index = 0;;
	int count = 0;
	for( ; pos_start != pos_end ; pos_start++ ){
		if( count == 0 )
			func_asm[index++] = *pos_start;
		if( *pos_start == '\t' )
			count--;
		else if( *pos_start == '\n' )
			count = 2;
	}
	func_asm[index++] = '\n';
	func_asm[index] = 0;
	return func_asm;
}

vector<string> traverse(string &filename,string &func,unordered_map<string,sym_entry*> &table){
	vector<string> out_funcs;
	if( lib_info_map.find(filename) == lib_info_map.end() || table.find(func) == table.end() || table[func]->st_shndx == SHN_UNDEF ){
		return out_funcs;
	}
	/*
	if( table[func]->type != STT_FUNC ){
		cerr << "ERROR 6: traversing non function symbol (" << func << "@" << filename << " type:" << (int)table[func]->type << ")" << endl;
		exit(-1);
	}
	*/
	Node* node = graph.find(func,filename);
	if( node == NULL ){
		cerr << "ERROR 7: traversing non existing node (" << func << "@" << filename << ")" << endl;
		exit(-1);
	}
	if( node->func_asm == NULL ){
		node->func_asm = extract_asm(lib_info_map[filename].lib_asm,table[func]);
	}
	const char * spos = node->func_asm;
	while( (spos = strstr(spos,"callq")) != NULL ){
		const char * new_line = strchr(spos,'\n');
		if( new_line == NULL ) break;
		string callstr(spos,new_line-spos);

		size_t pound = callstr.find_first_of('#');
		size_t lb = callstr.find_first_of('<');
		if( lb == callstr.npos || (pound != callstr.npos && pound < lb) ){
			spos = new_line;
			continue;
		}
		size_t rb = callstr.find_first_of('>');
		callstr = callstr.substr(lb+1,rb-lb-1);
		if( callstr.substr(0,5) == "*ABS*" ){
			spos = new_line;
			continue;
		}	
		size_t atsign = callstr.find_first_of('@');
		if( atsign != callstr.npos ){
			callstr = callstr.substr(0,atsign);
		}
		size_t plussign = callstr.find_first_of('+');
		if( plussign != callstr.npos ){
			callstr = callstr.substr(0,plussign);
		}
		bool has_func = false;
		for( int j = 0 ; j < out_funcs.size() ; j++ ){
			if( out_funcs[j] == callstr ){
				has_func = true;
				break;
			}
		}
		if( !has_func ){
			out_funcs.push_back(callstr);
		}
		spos = new_line;
	}
	return out_funcs;
}
/*
void getshstrtab(ifstream &infile,char* &shstr,Elf64_Xword &shstrsize){
	Elf64_Off shoff;
	Elf64_Half shentsize;
	Elf64_Half shnum;
	Elf64_Half shstrndx;
	Elf64_Off shstrtab;
	//16+2+2+4+8+8=40
	infile.seekg(40);
	infile.read((char *)&shoff,sizeof(shoff));
	//40+8+4+2+2+2=58
	infile.seekg(58);
	infile.read((char *)&shentsize,sizeof(shentsize));
	infile.read((char *)&shnum,sizeof(shnum));
	infile.read((char *)&shstrndx,sizeof(shstrndx));
	infile.seekg(shoff+shentsize*shstrndx+2*sizeof(Elf64_Word)+sizeof(Elf64_Xword)+sizeof(Elf64_Addr));
	infile.read((char *)&shstrtab,sizeof(shstrtab));
	infile.read((char *)&shstrsize,sizeof(shstrsize));
	infile.seekg(shstrtab);
	shstr = new char[shstrsize];
	infile.read((char *)shstr,shstrsize);
}
*/

/***********************************************************************
	Get File Lib Dependency
***********************************************************************/
vector<string> getdependlib(string &filename){
	vector<string> dependency;
	ifstream infile(filename);
	if( !infile )
		return dependency;

	Elf64_Off shoff;
	Elf64_Half shentsize;
	Elf64_Half shnum;
	Elf64_Half shstrndx;
	Elf64_Off shstrtaboff;
	Elf64_Xword shstrsize;

	infile.seekg(E_SHOFF_OFFSET);
	infile.read((char *)&shoff,sizeof(shoff));

	infile.seekg(E_SHENTSIZE_OFFSET);
	infile.read((char *)&shentsize,sizeof(shentsize));
	infile.read((char *)&shnum,sizeof(shnum));
	infile.read((char *)&shstrndx,sizeof(shstrndx));

	infile.seekg(shoff+shentsize*shstrndx+SH_OFFSET_OFFSET);
	infile.read((char *)&shstrtaboff,sizeof(shstrtaboff));
	infile.read((char *)&shstrsize,sizeof(shstrsize));

	char* shstr = new char[shstrsize];
	infile.seekg(shstrtaboff);
	infile.read((char *)shstr,shstrsize);

	for( int i = 0 ; i < shnum ; i++ ){
		Elf64_Word sh_name_index;
		infile.seekg(shoff+shentsize*i);
		infile.read((char *)&sh_name_index,sizeof(sh_name_index));
		if( string(".dynamic").compare(&shstr[sh_name_index]) == 0 ){
			Elf64_Off dynoff;
			Elf64_Xword dynsize;
			Elf64_Word dynlink;
			Elf64_Xword dynentsize;
			Elf64_Off dynastroff;
			Elf64_Xword dynastrsize;

			infile.seekg(shoff+shentsize*i+SH_OFFSET_OFFSET);
			infile.read((char *)&dynoff,sizeof(dynoff));
			infile.read((char *)&dynsize,sizeof(dynsize));
			infile.read((char *)&dynlink,sizeof(dynlink));

			infile.seekg(shoff+shentsize*i+SH_ENTSIZE_OFFSET);
			infile.read((char *)&dynentsize,sizeof(dynentsize));


			infile.seekg(shoff+shentsize*dynlink+SH_OFFSET_OFFSET);
			infile.read((char *)&dynastroff,sizeof(dynastroff));
			infile.read((char *)&dynastrsize,sizeof(dynastrsize));

			dynsize /= dynentsize;
			char * dynastr = new char[dynastrsize];
			infile.seekg(dynastroff);
			infile.read((char *)dynastr,dynastrsize);

			for( int j = 0 ; j < dynsize ; j++ ){
				Elf64_Sxword d_tag;
				infile.seekg(dynoff+dynentsize*j);
				infile.read((char *)&d_tag,sizeof(d_tag));
				if( d_tag == DT_NEEDED ){
					Elf64_Xword d_val;
					infile.read((char *)&d_val,sizeof(d_val));
					dependency.push_back(string(&dynastr[d_val]));
				}
			}
			delete[] dynastr;
			return dependency;
		}
	}
	return dependency;
}


/***********************************************************************
	Get dynamic symbol table
***********************************************************************/
int getdynsym(string filename,sym_entry* &dynsym,Elf64_Xword &dynsymsize){
	ifstream infile(filename);
	if( !infile )
		return GETDYNSYM_FAIL;
	Elf64_Off shoff;
	Elf64_Half shentsize;
	Elf64_Half shnum;
	Elf64_Half shstrndx;
	Elf64_Off shstrtaboff;
	Elf64_Xword shstrsize;

	infile.seekg(E_SHOFF_OFFSET);
	infile.read((char *)&shoff,sizeof(shoff));
	infile.seekg(E_SHENTSIZE_OFFSET);
	infile.read((char *)&shentsize,sizeof(shentsize));
	infile.read((char *)&shnum,sizeof(shnum));
	infile.read((char *)&shstrndx,sizeof(shstrndx));

	infile.seekg(shoff+shentsize*shstrndx+SH_OFFSET_OFFSET);
	infile.read((char *)&shstrtaboff,sizeof(shstrtaboff));
	infile.read((char *)&shstrsize,sizeof(shstrsize));

	char* shstr = new char[shstrsize];
	infile.seekg(shstrtaboff);
	infile.read((char *)shstr,shstrsize);

	int dynsym_index = -1;
	for( int i = 0 ; i < shnum ; i++ ){
		Elf64_Word sh_name_index;
		infile.seekg(shoff+shentsize*i);
		infile.read((char *)&sh_name_index,sizeof(sh_name_index));
		if( string(".dynsym").compare(&shstr[sh_name_index]) == 0 ){
			dynsym_index = i;
			break;
		}
	}
	if( dynsym_index != -1 ){
		Elf64_Off dynsymoff;
		Elf64_Xword dynsymentsize;
		Elf64_Word dynsymlink;
		Elf64_Off dynstroff;
		Elf64_Xword dynstrsize;

		infile.seekg(shoff+shentsize*dynsym_index+SH_OFFSET_OFFSET);
		infile.read((char *)&dynsymoff,sizeof(dynsymoff));
		infile.read((char *)&dynsymsize,sizeof(dynsymsize));
		infile.read((char *)&dynsymlink,sizeof(dynsymlink));

		infile.seekg(shoff+shentsize*dynsym_index+SH_ENTSIZE_OFFSET);
		infile.read((char *)&dynsymentsize,sizeof(dynsymentsize));

		infile.seekg(shoff+shentsize*dynsymlink+SH_OFFSET_OFFSET);
		infile.read((char *)&dynstroff,sizeof(dynstroff));
		infile.read((char *)&dynstrsize,sizeof(dynstrsize));

		char * dynstr = new char[dynstrsize];
		infile.seekg(dynstroff);
		infile.read((char *)dynstr,dynstrsize);

		dynsymsize /= dynsymentsize;
		dynsym = new sym_entry[dynsymsize];
		for( int j = 0 ; j < dynsymsize ; j++ ){
			Elf64_Word st_name;

			infile.seekg(dynsymoff+dynsymentsize*j);
			infile.read((char *)&st_name,sizeof(st_name));
			dynsym[j].name = string(&dynstr[st_name]);
			size_t pos;
			if( (pos = dynsym[j].name.find('@')) != dynsym[j].name.npos ){
				dynsym[j].name = dynsym[j].name.substr(0,pos);
			}
			unsigned char c;
			infile.read((char *)&c,sizeof(c));
			dynsym[j].bind = c>>4;
			dynsym[j].type = c&0xf;
			infile.read((char *)&c,sizeof(c));
			infile.read((char *)&dynsym[j].st_shndx,sizeof(dynsym[j].st_shndx));
			infile.read((char *)&dynsym[j].st_value,sizeof(dynsym[j].st_value));
		}
		delete[] dynstr;
		return GETDYNSYM_SUCCESS;
	}else{
		return GETDYNSYM_FAIL;
	}
}

/***********************************************************************
	Load lib function	
***********************************************************************/
int loadlib(string lib_name){
	if( lib_info_map.find(lib_name) != lib_info_map.end() ){
		return LOADLIB_EXIST;
	}
	string lib_path;
	if( !find_lib(lib_name,lib_path) ){
		cerr << "ERROR 2: " << lib_name << " not exit" << endl;
		return LOADLIB_FAIL;
	}

	sym_entry* symtab;
	Elf64_Xword symtabsize;
	char* lib_asm;
	if( getsym(lib_path,symtab,symtabsize) != GETSYM_SUCCESS ){
		cerr << "ERROR 3: fail to load symbol table(" << lib_name << ")" << endl; 
		exit(-1);
	}
	if( (lib_asm = readasm(lib_path)) == NULL ){
		cerr << "ERROR 4: fail to load asm text(" << lib_name << ")" << endl;
		exit(-1);
	}
	
	lib_info li(symtab,symtabsize,lib_asm);
	lib_info_map[lib_name] = li;
	for( int i = 0 ; i < symtabsize ; i++ ){
		lib_info_map[lib_name].table[symtab[i].name] = &symtab[i];
	}

	lib_info_map[lib_name].dependency = getdependlib(lib_path);
	return LOADLIB_SUCCESS;
}


/***********************************************************************
	Complete lib path if lib exist	
***********************************************************************/
bool find_lib(string lib_name,string &lib_path){
	for( int i = 0 ; i < default_path.size() ; i++ )
		if(ifstream(default_path[i]+"/"+lib_name)){
			lib_path = default_path[i]+"/"+lib_name;
			return true;
		}
	return false;
}

/***********************************************************************
	Return objdump text buffer
***********************************************************************/
char* readasm(string filename){
	char* buffer = NULL;
	int buffersize;
	system(string("objdump -d " + filename + " > asm_temp.txt").c_str());
	ifstream asm_in("asm_temp.txt");
	if( !asm_in )
		return NULL;
	asm_in.seekg(0,ifstream::end);
	buffersize = asm_in.tellg();
	buffersize += 2;
	asm_in.seekg(0,ifstream::beg);
	buffer = new char[buffersize];
	asm_in.read((char *)buffer,buffersize-2);
	buffer[buffersize-2] = '\n';
	buffer[buffersize-1] = 0;
	return buffer;
}

/***********************************************************************
	Get symbol table
***********************************************************************/
int getsym(string filename,sym_entry* &symtab,Elf64_Xword &symtabsize){
	ifstream infile(filename);
	if( !infile )
		return GETSYM_NOFILE;

	Elf64_Off shoff;
	Elf64_Half shentsize;
	Elf64_Half shnum;
	Elf64_Half shstrndx;
	Elf64_Off shstrtaboff;
	Elf64_Xword shstrsize;
	char * shstr;

	infile.seekg(E_SHOFF_OFFSET);
	infile.read((char *)&shoff,sizeof(shoff));

	infile.seekg(E_SHENTSIZE_OFFSET);
	infile.read((char *)&shentsize,sizeof(shentsize));
	infile.read((char *)&shnum,sizeof(shnum));
	infile.read((char *)&shstrndx,sizeof(shstrndx));

	infile.seekg(shoff+shentsize*shstrndx+SH_OFFSET_OFFSET);
	infile.read((char *)&shstrtaboff,sizeof(shstrtaboff));
	infile.read((char *)&shstrsize,sizeof(shstrsize));

	shstr = new char[shstrsize];
	infile.seekg(shstrtaboff);
	infile.read((char *)shstr,shstrsize);

	int symtab_index = -1;
	int debuglink_index = -1;
	for( int i = 0 ; i < shnum ; i++ ){
		Elf64_Word sh_name_index;
		infile.seekg(shoff+shentsize*i);
		infile.read((char *)&sh_name_index,sizeof(sh_name_index));
		if( string(".symtab").compare(&shstr[sh_name_index]) == 0 ){
			symtab_index = i;
			break;
		}else if( string(".gnu_debuglink").compare(&shstr[sh_name_index]) == 0 ){
			debuglink_index = i;
		}
	}

	if( symtab_index != -1 ){
		Elf64_Off symtaboff;
		Elf64_Xword symtabentsize;
		Elf64_Word symtablink;
		Elf64_Off symstroff;
		Elf64_Xword symstrsize;

		infile.seekg(shoff+shentsize*symtab_index+SH_OFFSET_OFFSET);
		infile.read((char *)&symtaboff,sizeof(symtaboff));
		infile.read((char *)&symtabsize,sizeof(symtabsize));
		infile.read((char *)&symtablink,sizeof(symtablink));

		infile.seekg(shoff+shentsize*symtab_index+SH_ENTSIZE_OFFSET);
		infile.read((char *)&symtabentsize,sizeof(symtabentsize));

		infile.seekg(shoff+shentsize*symtablink+SH_OFFSET_OFFSET);
		infile.read((char *)&symstroff,sizeof(symstroff));
		infile.read((char *)&symstrsize,sizeof(symstrsize));

		char * symstr = new char[symstrsize];
		infile.seekg(symstroff);
		infile.read((char *)symstr,symstrsize);

		symtabsize /= symtabentsize;
		symtab = new sym_entry[symtabsize];
		for( int j = 0 ; j < symtabsize ; j++ ){
			Elf64_Word st_name;

			infile.seekg(symtaboff+symtabentsize*j);
			infile.read((char *)&st_name,sizeof(st_name));
			symtab[j].name = string(&symstr[st_name]);
			size_t pos;
			if( (pos = symtab[j].name.find('@')) != symtab[j].name.npos ){
				symtab[j].name = symtab[j].name.substr(0,pos);
			}
			unsigned char c;
			infile.read((char *)&c,sizeof(c));
			symtab[j].bind = c>>4;
			symtab[j].type = c&0xf;
			infile.read((char *)&c,sizeof(c));
			infile.read((char *)&symtab[j].st_shndx,sizeof(symtab[j].st_shndx));
			infile.read((char *)&symtab[j].st_value,sizeof(symtab[j].st_value));
		}
		delete[] symstr;
		infile.close();
		return GETSYM_SUCCESS;
	}else if( debuglink_index != -1 ){
		Elf64_Off debuglink_off;
		string debug_name;

		infile.seekg(shoff+shentsize*debuglink_index+SH_OFFSET_OFFSET);
		infile.read((char *)&debuglink_off,sizeof(debuglink_off));
		infile.seekg(debuglink_off);
		
		size_t pos = filename.find_last_of("/");
		debug_name = filename.substr(0,pos+1);
		debug_name.insert(0,string(debug_dir));
		char c;
		infile.read((char *)&c,sizeof(c));
		while( c != 0 ){
			debug_name.append(1,c);
			infile.read((char *)&c,sizeof(c));
		}
		infile.close();

		if( getsym(debug_name,symtab,symtabsize) == GETSYM_SUCCESS )
			return GETSYM_SUCCESS;
		else
			return GETSYM_NODBG;
	}else{
		infile.close();
		return GETSYM_NOSYMDBG;
	}
}
/*
bool getdebuglink(ifstream &infile,string &debug_file,char *shstr){
	Elf64_Off shoff;
	Elf64_Half shentsize;
	Elf64_Half shnum;
	infile.seekg(40);
	infile.read((char *)&shoff,sizeof(shoff));
	infile.seekg(58);
	infile.read((char *)&shentsize,sizeof(shentsize));
	infile.read((char *)&shnum,sizeof(shnum));
	for( int i = 0 ; i < shnum ; i++ ){
		Elf64_Word sh_name_index;
		infile.seekg(shoff+shentsize*i);
		infile.read((char *)&sh_name_index,sizeof(sh_name_index));
		if( string(".gnu_debuglink").compare(&shstr[sh_name_index]) == 0 ){
			Elf64_Off link_off;
			Elf64_Xword link_size;
			infile.seekg(shoff+shentsize*i+2*sizeof(Elf64_Word)+sizeof(Elf64_Xword)+sizeof(Elf64_Addr));
			infile.read((char *)&link_off,sizeof(link_off));
			infile.read((char *)&link_size,sizeof(link_size));
			infile.seekg(link_off);
			char c;
			infile.read((char *)&c,sizeof(c));
			while( c != 0 ){
				debug_file.append(1,c);
				infile.read((char *)&c,sizeof(c));
			}
			return true;
		}
	}
	return false;
}
*/

//DEBUG FUNCTIONS
void display_strtab(char* &strtab,Elf64_Xword &strtabsize,const char * title){
	cerr << endl << title << endl;
	for( int i = 0 ; i < strtabsize ; i++ ){
		if( strtab[i] == 0 ) cerr << endl;
		else cerr << strtab[i];
	}
}

void display_symtab(lib_info &lib,const char * title){
	sym_entry* symtab = lib.symtab;
	Elf64_Xword symtabsize = lib.symtabsize;
	cout << endl << title << endl;
	cout << left << setw(10) << "type" << setw(10) << "bind" << setw(10) << "shndx" << setw(15) << "st_value" << "name" << endl;
	for( int i = 0 ; i < symtabsize ; i++ ){
		cout << left << setw(10);
		switch(symtab[i].type){
			case STT_NOTYPE	:cout << "NOTYPE";	break;
			case STT_OBJECT	:cout << "OBJECT";	break;
			case STT_FUNC	:cout << "FUNC";	break;
			case STT_SECTION:cout << "SECTION";	break;
			case STT_FILE	:cout << "FILE";	break;
			case STT_LOOS	:cout << "-";		break;
			case STT_HIOS	:cout << "-";		break;
			case STT_LOPROC	:cout << "-";		break;
			case STT_HIPROC	:cout << "-";		break;
			default: cout << " "; break;
		}
		cout << setw(10);
		switch(symtab[i].bind){
			case STB_LOCAL	:cout << "LOCAL";	break;
			case STB_GLOBAL :cout << "GLOBAL";	break;
			case STB_WEAK	:cout << "WEAK";	break;
			case STB_LOOS	:cout << "-";		break;
			case STB_HIOS	:cout << "-";		break;
			case STB_LOPROC	:cout << "-";		break;
			case STB_HIPROC	:cout << "-";		break;
			default: cout << " "; break;
		}
		cout << setw(10);
		switch(symtab[i].st_shndx){
			case SHN_UNDEF	:cout << "UND";		break;
			case SHN_LOPROC	:cout << "LOPROC";	break;
			case SHN_HIPROC	:cout << "HIPROC";	break;
			case SHN_LOOS	:cout << "LOOS";	break;
			case SHN_HIOS	:cout << "HIOS";	break;
			case SHN_ABS	:cout << "ABS";		break;
			case SHN_COMMON	:cout << "COMMON";	break;
			default			:cout << symtab[i].st_shndx;break;
		}
		cout << setw(15) << symtab[i].st_value;
		cout << symtab[i].name << endl;
	}
}
void show_trace(string in_func,vector<string> &out_funcs){
	cout << in_func << ":" << endl;
	for( int i = 0 ; i < out_funcs.size() ; i++ ){
		cout << "     " << out_funcs[i] << endl;
	}
	cout << endl;
}
void display_dependency(lib_info &lib,const char* title){
	cout << endl << title << endl;
	for( int i = 0 ; i < lib.dependency.size() ; i++ ){
		cout << lib.dependency[i] << endl;
	}
}

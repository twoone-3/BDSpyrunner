#include "pch.h"
#include "Detours/include/detours.h"
using namespace std;
VA BKDR(const char* _ch, size_t sz) {
	unsigned char* ch = (unsigned char*)_ch;
	VA rv = 0;
	for (int i = 0; i < sz; ++i) {
		rv = rv * 127 - 36 + ch[i];
	}
	return rv;
}
struct hashval {
	string key;
	VA hash;
	string data;
	hashval() { hash = 0; }
	hashval(string&& a, VA b, string&& c) : key(forward<string>(a)), hash(b), data(forward<string>(c)) {}
};
struct RoDB_R {
	ifstream fp;
	unsigned int cnt_bucket;
	unsigned int bucket[4096]; //offset
	unsigned int data_off;
	RoDB_R(const char* path) {
		fp.open(path, ios::binary);
#define ppch(x) ((char*)&x)
		fp.read(ppch(cnt_bucket), 4);
		fp.read(ppch(data_off), 4);
		fp.read(ppch(bucket[0]), 4 * (size_t)cnt_bucket);
	}
	void read(void* buf, unsigned int off, size_t sz) {
		fp.seekg(streampos(off));
		fp.read((char*)buf, sz);
	}
	void getstr(string& buf, unsigned int off) {
		if (off != 0) {
			fp.seekg(streampos(off));
		}
		char ch;
		while ((ch = fp.get()) != 0)
			buf.append(1, ch);
	}
	string val2key(unsigned int rva) {
		fp.seekg(streampos(data_off));
		string name;
		name.reserve(8192);
		while (!fp.eof()) {
			int ch = fp.get();
			if (ch == 0) {
				unsigned int dst;
				fp.read((char*)&dst, 4);
				if (dst == rva) {
					return name;
				}
				else {
					name.clear();
				}
			}
			else {
				name.push_back(ch);
			}
		}
		return "(nil)";
	}
	bool _cmp(string_view key) {
		for (uint32_t i = 0; i < key.size(); ++i) {
			int ch = fp.get();
			if (ch != key[i]) {
				return false;
			}
		}
		return fp.get() == 0;
	}
	unsigned int get(string_view key) { //return file offset
		auto hash = BKDR(key.data(), key.size());
		auto bkoff = bucket[hash % cnt_bucket];
		fp.seekg(streampos(bkoff));
		vector<unsigned int> tolookup;
		tolookup.reserve(512);
		while (1) {
			VA hs;
			unsigned int off;
			fp.read(ppch(hs), 8);
			fp.read(ppch(off), 4);
			if (off == 0xffffffff)
				break;
			if (hs == hash)
				tolookup.push_back(off + data_off);
			/*
			if (hs == hash) {
				off += data_off;
				auto pos = fp.tellg();
				string key_now;
				getstr(key_now, off);
				if (key == key_now) {
					return off + (unsigned int)key.size() + 1;
				}
				fp.seekg(pos);
			}*/
		}
		for (auto off : tolookup) {
			fp.seekg({ off });
			if (_cmp(key)) {
				return off + (unsigned)key.size() + 1;
			}
		}
		return 0;
	}
};
void* GetServerSymbol(const char* x) {
	static RoDB_R pdb = "bedrock_server.symdb";
	static VA BaseAdr = (VA)GetModuleHandle(0);
	unsigned rv = pdb.get(x);
	if (!rv)
		return nullptr;
	unsigned rva;
	pdb.read(&rva, rv, 4);
	return (void*)(BaseAdr + rva);
}
void* HookFunction(void* oldfunc, void* newfunc) {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&oldfunc, newfunc);
	DetourTransactionCommit();
	return oldfunc;
}
// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "dllmain.h"

FILE *f;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


BOOL __cdecl load(HGLOBAL h, long len){
	char* buf = new char[len + 1];
	memcpy(buf, h, len);
	buf[len] = 0;
	string param((char*)buf);
	GlobalFree(h);
	delete[] buf;

	SetCurrentDirectory(s2w(param).c_str());

	FILE* f;
	fopen_s(&f, "index.rb", "rb");
	mrb = mrb_open();
	if (!mrb) {
		return false;
	}

	mrb_load_file(mrb, f);
	fclose(f);

	mrb_value r = mrb_funcall(mrb, mrb_top_self(mrb), "_load", 1,
		mrb_str_new(mrb, param.c_str(), param.length())
		);
	return mrb_bool(r);
}


BOOL __cdecl unload(){
	mrb_value r = mrb_funcall(mrb, mrb_top_self(mrb), "_unload", 0);
	bool ret = mrb_bool(r);
	mrb_close(mrb);
	return false;
}
HGLOBAL __cdecl request(HGLOBAL h, long *len){

	char* buf = new char[*len + 1];
	memcpy(buf, h, *len);
	buf[*len] = 0;
	string param((char*)buf);
	GlobalFree(h);
	delete[] buf;

	mrb_value r = mrb_funcall(mrb, mrb_top_self(mrb), "_request", 1,
		mrb_str_new(mrb, param.c_str(), param.length())
		);

	string ret=string(RSTRING_PTR(r));

	*len = ret.size() + 1;
	buf = (char*)GlobalAlloc(GMEM_FIXED, *len);
	memcpy(buf, ret.c_str(), *len);
	//	((char*)buf)[*len - 1] = 0;
	return buf;
}


wstring s2w(string in, string cs){
	UINT csint;
	if (cs == "Shift_JIS"){
		csint = 932;
	}
	else if (cs == "ISO-2022-JP"){
		csint = 50221;
	}
	else if (cs == "EUC-JP"){
		csint = 51932;
	}
	else if (cs == "UTF-8"){
		csint = 65001;
	}
	else{
		csint = CP_ACP;
	}
	const int nSize = MultiByteToWideChar(csint, 0, (LPCSTR)in.c_str(), -1, NULL, 0);
	TCHAR* buff = new TCHAR[nSize + 1];
	MultiByteToWideChar(csint, 0, (LPCSTR)in.c_str(), -1, (LPWSTR)buff, nSize);
	wstring ret(buff);
	delete[] buff;
	return ret;
}

string w2s(wstring in, string cs){
	UINT csint;
	if (cs == "Shift_JIS"){
		csint = 932;
	}
	else if (cs == "ISO-2022-JP"){
		csint = 50221;
	}
	else if (cs == "EUC-JP"){
		csint = 51932;
	}
	else if (cs == "UTF-8"){
		csint = 65001;
	}
	else{
		csint = CP_ACP;
	}
	const int nSize = WideCharToMultiByte(csint, 0, (LPCWSTR)in.c_str(), -1, NULL, 0, NULL, NULL);
	BYTE* buff = new BYTE[nSize + 1];
	WideCharToMultiByte(csint, 0, (LPCWSTR)in.c_str(), -1, (LPSTR)buff, nSize, NULL, NULL);
	string ret = string((char*)buff);
	delete[] buff;
	return ret;
}
// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための 
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された TVROCKRESIZERDLL_EXPORTS
// シンボルでコンパイルされます。このシンボルは、この DLL を使うプロジェクトで定義することはできません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、 
// TVROCKRESIZERDLL_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef TVROCKRESIZERDLL_EXPORTS
#define TVROCKRESIZERDLL_API __declspec(dllexport)
#define TVROCKRESIZERDLL_FUNC extern "C" __declspec(dllexport)
#else
#define TVROCKRESIZERDLL_API __declspec(dllimport)
#define TVROCKRESIZERDLL_FUNC extern "C" __declspec(dllexport)
#endif


//TVROCKRESIZERDLL_FUNC int TvRock_InjectDll(BOOL bInject, HANDLE *phProcess);

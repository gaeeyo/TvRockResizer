// �ȉ��� ifdef �u���b�N�� DLL ����̃G�N�X�|�[�g��e�Ղɂ���}�N�����쐬���邽�߂� 
// ��ʓI�ȕ��@�ł��B���� DLL ���̂��ׂẴt�@�C���́A�R�}���h ���C���Œ�`���ꂽ TVROCKRESIZERDLL_EXPORTS
// �V���{���ŃR���p�C������܂��B���̃V���{���́A���� DLL ���g���v���W�F�N�g�Œ�`���邱�Ƃ͂ł��܂���B
// �\�[�X�t�@�C�������̃t�@�C�����܂�ł��鑼�̃v���W�F�N�g�́A 
// TVROCKRESIZERDLL_API �֐��� DLL ����C���|�[�g���ꂽ�ƌ��Ȃ��̂ɑ΂��A���� DLL �́A���̃}�N���Œ�`���ꂽ
// �V���{�����G�N�X�|�[�g���ꂽ�ƌ��Ȃ��܂��B
#ifdef TVROCKRESIZERDLL_EXPORTS
#define TVROCKRESIZERDLL_API __declspec(dllexport)
#define TVROCKRESIZERDLL_FUNC extern "C" __declspec(dllexport)
#else
#define TVROCKRESIZERDLL_API __declspec(dllimport)
#define TVROCKRESIZERDLL_FUNC extern "C" __declspec(dllexport)
#endif


//TVROCKRESIZERDLL_FUNC int TvRock_InjectDll(BOOL bInject, HANDLE *phProcess);

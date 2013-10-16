
#include "MouseCursor.h"
#include "WindowFormUnit.h"

const LPTSTR MouseCursor::CURSORS[CURSOR_EOT] = {
	IDC_APPSTARTING,
	IDC_ARROW,
	IDC_CROSS,
	IDC_HAND,
	IDC_IBEAM,
	IDC_HELP,
	IDC_NO,
	IDC_SIZEALL,
	IDC_SIZENESW,
	IDC_SIZENS,
	IDC_SIZENWSE,
	IDC_SIZEWE,
	IDC_UPARROW,
	IDC_WAIT,
};

const int MouseCursor::CURSOR_INDEXES[MouseCursor::CURSOR_INDEXES_NUM] = {
	CURSOR_SIZEALL, // crSizeAll = -22,
	CURSOR_HAND, // crHandPoint = -21,
	CURSOR_HELP, // crHelp = -20,
	CURSOR_ARROW, // crAppStart = -19,
	CURSOR_NO, // crNo = -18,
	CURSOR_ARROW, // crSQLWait = -17,
	CURSOR_ARROW, // crMultiDrag = -16,
	CURSOR_ARROW, // crVSplit = -15,
	CURSOR_ARROW, // crHSplit = -14,
	CURSOR_ARROW, // crNoDrop = -13,
	CURSOR_ARROW, // crDrag = -12,
	CURSOR_WAIT, // crHourGlass = -11,
	CURSOR_UPARROW, // crUpArrow = -10,
	CURSOR_SIZEWE, // crSizeWE = -9,
	CURSOR_SIZENWSE, // crSizeNWSE = -8,
	CURSOR_SIZENS, // crSizeNS = -7,
	CURSOR_SIZENESW, // crSizeNESW = -6,
	CURSOR_SIZEALL, // crSize = -5,
	CURSOR_IBEAM, // crIBeam = -4,
	CURSOR_CROSS, // crCross = -3,
	CURSOR_ARROW, // crArrow = -2,
	-1, // crNone = -1,
	CURSOR_ARROW, // crDefault = 0,
	CURSOR_ARROW, // crHBeam = 1,
};
std::vector<HCURSOR> MouseCursor::CURSOR_HANDLES_FOR_INDEXES;
HCURSOR MouseCursor::CURSOR_HANDLES[CURSOR_EOT];
bool MouseCursor::CURSOR_INITIALIZED = false;
bool MouseCursor::is_cursor_hide_ = false;

void MouseCursor::Initialize() {
	if( CURSOR_INITIALIZED ) return;

	// �f�t�H���g�J�[�\����ǂݍ���
	for( int i = 0; i < CURSOR_EOT; i++ ) {
		CURSOR_HANDLES[i] = ::LoadCursor( NULL, CURSORS[i] );
	}

	// ���J�J�[�\��ID�ƈ�v����悤�Ƀn���h�����i�[����
	CURSOR_HANDLES_FOR_INDEXES.clear();
	CURSOR_HANDLES_FOR_INDEXES.reserve( CURSOR_INDEXES_NUM );
	for( int i = 0; i < CURSOR_INDEXES_NUM; i++ ) {
		int index = CURSOR_INDEXES[i];
		if( index >= 0 ) {
			CURSOR_HANDLES_FOR_INDEXES.push_back( CURSOR_HANDLES[index] );
		} else {
			CURSOR_HANDLES_FOR_INDEXES.push_back( INVALID_HANDLE_VALUE );
		}
	}
	CURSOR_INITIALIZED = true;
}
void MouseCursor::Finalize() {
	// �ǂݍ��܂ꂽ�J�[�\�����폜
	if( CURSOR_HANDLES_FOR_INDEXES.size() > CURSOR_INDEXES_NUM ) {
		int numCursor = CURSOR_HANDLES_FOR_INDEXES.size();
		for( int i = CURSOR_INDEXES_NUM; i < numCursor; i++ ) {
			::DestroyCursor( CURSOR_HANDLES_FOR_INDEXES[i] );
			CURSOR_HANDLES_FOR_INDEXES[i] = INVALID_HANDLE_VALUE;
		}
	}
}
void MouseCursor::UpdateCurrentCursor() {
	int oldindex = cursor_index_;
	cursor_index_ = INVALID_CURSOR_INDEX;
	HCURSOR hCursor = ::GetCursor();

	if( oldindex >= 0 && CURSOR_HANDLES_FOR_INDEXES[oldindex] == hCursor ) {
		// �܂��͋��J�[�\���Ɣ�r
		cursor_index_ = oldindex; // �ύX����Ă��Ȃ�
	} else if( CURSOR_HANDLES[CURSOR_ARROW] == hCursor ) {
		// ���Ƀf�t�H���g�Ɣ�r
		cursor_index_ = crDefault;
	} else {
		// �ȑO�ƈقȂ�A�f�t�H���g�ł��Ȃ����́A�S�ĂƔ�r����
		for( int i = 0; i < CURSOR_EOT; i++ ) {
			if( CURSOR_HANDLES_FOR_INDEXES[i] == hCursor ) {
				cursor_index_ = i;
				break;
			}
		}
	}
}
void MouseCursor::SetMouseCursor( int index ) {
	if( is_cursor_hide_ && index != crNone ) {
		::ShowCursor( TRUE );
		is_cursor_hide_ = false;
	}
	if( index == crNone ) {
		if( is_cursor_hide_ != true ) {
			::ShowCursor( FALSE );
			is_cursor_hide_ = true;
		}
	} else {
		int id = index + CURSOR_OFFSET; // crSizeAll = -22, �Ȃ̂ŁA���̕����Z
		if( id >= 0 && id < (int)CURSOR_HANDLES_FOR_INDEXES.size() ) {
			::SetCursor( CURSOR_HANDLES_FOR_INDEXES[id] );
		} else {
			::SetCursor( CURSOR_HANDLES[CURSOR_ARROW] );
		}
	}
/*
	switch( index ) {
	case crDefault:
		::SetCursor( CURSOR_HANDLES[CURSOR_ARROW] );
		break;
	case crNone:
		if( is_cursor_hide_ != true ) {
			::ShowCursor( FALSE );
			is_cursor_hide_ = true;
		}
		break;
	case crArrow:
		::SetCursor( CURSOR_HANDLES[CURSOR_ARROW] );
		break;
	case crCross:
		::SetCursor( CURSOR_HANDLES[CURSOR_CROSS] );
		break;
	case crIBeam:
		::SetCursor( CURSOR_HANDLES[CURSOR_IBEAM] );
		break;
	case crSize:
		::SetCursor( CURSOR_HANDLES[CURSOR_SIZEALL] );
		break;
	case crSizeNESW:
		::SetCursor( CURSOR_HANDLES[CURSOR_SIZENESW] );
		break;
	case crSizeNS:
		::SetCursor( CURSOR_HANDLES[CURSOR_SIZENS] );
		break;
	case crSizeNWSE:
		::SetCursor( CURSOR_HANDLES[CURSOR_SIZENWSE] );
		break;
	case crSizeWE:
		::SetCursor( CURSOR_HANDLES[CURSOR_SIZEWE] );
		break;
	case crUpArrow:
		::SetCursor( CURSOR_HANDLES[CURSOR_UPARROW] );
		break;
	case crHourGlass:
		::SetCursor( CURSOR_HANDLES[CURSOR_WAIT] );
		break;
	case crHelp:
		::SetCursor( CURSOR_HANDLES[CURSOR_HELP] );
		break;
	case crHandPoint:
		::SetCursor( CURSOR_HANDLES[CURSOR_HAND] );
		break;
	case crSizeAll:
		::SetCursor( CURSOR_HANDLES[CURSOR_SIZEALL] );
		break;
	case crNo:
		::SetCursor( CURSOR_HANDLES[CURSOR_NO] );
		break;
	case crDrag:
	case crNoDrop:
	case crHSplit:
	case crVSplit:
	case crMultiDrag:
	case crSQLWait:
	case crAppStart:
	case crHBeam:
	default:
		::SetCursor( CURSOR_HANDLES[CURSOR_ARROW] );
		break;
	}
*/
}
void MouseCursor::SetCursor() {
	if( cursor_index_ != INVALID_CURSOR_INDEX ) {
		SetMouseCursor( cursor_index_ );
	}
}

void MouseCursor::SetCursorIndex( int index ) {
	cursor_index_ = index;
}
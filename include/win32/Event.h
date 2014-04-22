/** @file
* @brief 同期オブジェクト・イベントの宣言
*
* @author 中村  薫
* $Date: 2004/10/21 11:25:09 $
* $Revision: 1.1.1.1 $
*/
#if !defined( EVENT_H_INCLUDE )
#define EVENT_H_INCLUDE

#include <memory>

namespace std {
	using namespace std::tr1;
}

// MFC使用時用にはwindows.hはインクルードしない
#if !defined( __AFXWIN_H__ )
#include <windows.h>
#endif  // !defined( __AFXWIN_H__ )

/// Win32ラッパークラス群
namespace win32
{
    /// イベント
    class Event
    {
    public:

        typedef std::shared_ptr< void > Handle;

        Event( LPCTSTR lpName, BOOL bManualReset = TRUE, BOOL bInitialState = FALSE,
               LPSECURITY_ATTRIBUTES lpEventAttributes = 0 );
        ~Event();

        bool Wait( DWORD dwMilliseconds = INFINITE );

        HANDLE get() { return handle_.get(); }

        // このイベントが既に開かれていたものかどうか
        bool IsExists() const { return exists_; }

    private:

        Handle handle_; ///< イベントハンドル
        bool   exists_; ///< このイベントが既に開かれていたものかどうか
    };

} // namespace win32

#endif // !defined( EVENT_H_INCLUDE )
// EOF

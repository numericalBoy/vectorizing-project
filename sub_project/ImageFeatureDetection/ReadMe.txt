﻿================================================================================
    MFC 程式庫：ImageFeatureDetection 專案概觀
===============================================================================

應用程式精靈已經為您建立了這個 ImageFeatureDetection 應用程式。這個應用程式不僅示範了使用 MFC 的基本概念，也是您撰寫應用程式的起點。

這個檔案包含一份摘要，簡要說明構成 ImageFeatureDetection 應用程式的
所有檔案，它們個別的內容。

ImageFeatureDetection.vcxproj
    這是使用應用程式精靈所產生之 VC++ 專案的主要專案檔。它含有產生該檔案之 Visual C++ 的版本資訊，以及有關使用應用程式精靈所選取之平台、組態和專案功能的資訊。

ImageFeatureDetection.vcxproj.filters
    這是使用應用程式精靈所產生之 VC++ 專案的篩選檔。檔案中含有您專案中檔案與篩選器之間關聯的相關資訊。這項關聯用於 IDE 中以顯示特定節點下具有類似副檔名之檔案的群組 (例如，".cpp" 檔案會與 "Source Files" 篩選器相關聯)。

ImageFeatureDetection.h
    這是應用程式的主要標頭檔。
    它包含了特定標頭 (包括 Resource.h)，並宣告 CImageFeatureDetectionApp 應用程式類別。

ImageFeatureDetection.cpp
    這是含有應用程式類別 CImageFeatureDetectionApp 的主要應用程式原始程式檔。

ImageFeatureDetection.rc
    這是程式所用的所有 Microsoft Windows 資源的列表。它含有儲存在 RES 子目錄中的圖示、點陣圖和游標。您可以直接在 Microsoft Visual C++ 內編輯這個檔案。您的專案資源是在 1028。

res\ImageFeatureDetection.ico
    這是用來做為應用程式圖示的圖示檔。這個圖示是包含在主要資源檔 ImageFeatureDetection.rc 中。

res\ImageFeatureDetection.rc2
    這個檔案包含 Microsoft Visual C++ 無法編輯的資源。您應該將所有資源編輯器無法編輯的資源放到這個檔案。

/////////////////////////////////////////////////////////////////////////////

對於主框架視窗：
    專案包括標準 MFC 介面。

MainFrm.h, MainFrm.cpp
    這些檔案包含了框架類別 CMainFrame，其衍生自
    CFrameWnd 並控制所有 SDI 框架功能。

res\Toolbar.bmp
    這個點陣圖檔案是用來建立工具列的並排影像。
    初始工具列和狀態列會在 CMainFrame 類別中建構。請使用資源編輯器編輯這個工具列點陣圖，並更新 ImageFeatureDetection.rc 中的 IDR_MAINFRAME TOOLBAR 陣列，以加入工具列按鈕。
/////////////////////////////////////////////////////////////////////////////

應用程式精靈會建立一個文件類型及一個檢視表：

ImageFeatureDetectionDoc.h、ImageFeatureDetectionDoc.cpp - 文件
    這些檔案包含您的 CImageFeatureDetectionDoc 類別。編輯這些檔案，可加入您的特殊文件類型並實作檔案儲存和載入 (透過 CImageFeatureDetectionDoc::Serialize)。

ImageFeatureDetectionView.h、ImageFeatureDetectionView.cpp - 文件的檢視表
    這些檔案包含您的 CImageFeatureDetectionView 類別。
    CImageFeatureDetectionView 物件是用來檢視 CImageFeatureDetectionDoc 物件。




/////////////////////////////////////////////////////////////////////////////

其他功能：

ActiveX 控制項
    應用程式包括使用 ActiveX 控制項的支援。

列印和預覽列印支援
    應用程式精靈藉由從 MFC 程式庫呼叫 CView 類別中的成員函式，產生處理列印、列印設定與列印預覽命令的程式碼。

/////////////////////////////////////////////////////////////////////////////

其他標準檔案：

StdAfx.h, StdAfx.cpp
    這些檔案是用來建置名為 ImageFeatureDetection.pch 的先行編譯標頭 (PCH) 檔，以及名為 StdAfx.obj 的先行編譯型別檔。

Resource.h
    這是標準標頭檔，它定義新的資源 ID。Microsoft Visual C++ 會讀取和更新這個檔案。

ImageFeatureDetection.manifest
	Windows XP 會使用應用程式資訊清單檔案來描述特定並存組件版本的應用程式相依性。載入器會使用這項資訊載入組件快取中的適當組件，或應用程式中的私用組件。應用程式資訊清單可能隨附用於轉散發，做為與應用程式可執行檔相同資料夾中所安裝的外部 .manifest 檔案，或是以資源的形式隨附在可執行檔中。
/////////////////////////////////////////////////////////////////////////////

其他注意事項:

應用程式精靈使用 "TODO:" 來指示您應該加入或自訂的原始程式碼部分。

如果您的應用程式使用 MFC 的共用 DLL，您將需要轉散發 MFC DLL。如果您的應用程式採用不同於作業系統地區設定的語言，您還必須轉散發對應的當地語系化資源 mfc110XXX.DLL。
如需這兩個主題的詳細資訊，請參閱 MSDN 文件中轉散發 Visual C++ 應用程式的章節。

/////////////////////////////////////////////////////////////////////////////

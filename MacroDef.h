#ifndef MACRODEF_H
#define MACRODEF_H

#define         DebugPath         L"/Debug"
#define         HlData            L"/HlData"
#define         JsonData          L"/jsonData"

#define         JsonFramework     L"/FrameWork"
#define         JsonStyle         L"/Style"
#define         JsonAutoRecord    L"/AutoRecord"
#define         JsonManualRecord  L"/ManualRecord"
#define         JsonMyTest        L"/MyTest"
#define         JsonVideoDb       L"/VideoDb"



#define         ModuleId          2017
#define			MODULE_CODE		  "F2017"
#define			ERROR_CODE		  "error_code"
#define         JsonFile          ".json"

#define			ERROR_OK		0



#define         LogFile           "SEGameHLController.log"

#define         LogStart          "Module:SEGameHLController,version"
#define         LogImage          "post game event, area cnt:"
#define         LogImageDetail    "post detail gametype:"
#define         PostAreaId        "areaID:"
#define         PostText          "info:"
#define         TimeSplit         " : "
#define         CommaSplit        ","
#define         SpaceSplit        " "
#define         CurYear           "2024"

#define         DataTimeFormat    "yyyy-MM-dd hh:mm:ss"

#define         ControlEnter      "\r\n"
#define         ControlEnterW     L"\r\n"
#define         TextEnter         "\n"



//json 字符串定义
/*
 * {"type":"post_hl","data":{"gameType":4,"areaInfo":[{"areaId":12,"info":"祝下次好运！"}]}}
 */
#define         JsType            "type"
#define         JsData            "data"
#define         JsGameType        "gameType"
#define         JsAreaInfo        "areaInfo"
#define         JsAreaId          "areaId"
#define         JsInfo            "info"
#define         JsPost_Hl         "post_hl"


 //Action define


//action name
#define			ACTION_BIGDATA						"BIGDATA"
#define			ACTION_MYTEST						"my_test"
#define			ACTION_ABILITY						"ability"
#define			ACTION_LOADED						"loaded"
#define			ACTION_UNINSTALL					"uninstall_resetsetting"
#define			ACTION_INIT							"init"
#define			ACTION_UNINIT						"uninit"
#define			ACTION_GAMINGAI_ENTER				"gamingai_enter"
//#define		ACTION_GAMINGAI_EXIT				"legion_exit_app"
#define			ACTION_GAMINGAI_EXIT				"gamingai_exit"
#define			ACTION_GET_HL_TIP					"get_hl_tip"
#define			ACTION_SET_HL_TIP					"set_hl_tip"
#define			ACTION_GET_HL_INFO					"get_hl_info"
#define			ACTION_SET_HL_INFO					"set_hl_info"
#define			ACTION_NOTIFY_HL_GVP				"notify_gen_video_progress"	
#define			ACTION_NOTIFY_RECORD_START			"notify_record_start"
#define			ACTION_NOTIFY_RECORD_FAILED			"notify_record_failed"
#define			ACTION_NOTIFY_RECORD_DONE			"notify_record_done"
#define			ACTION_DELETE_VIDEO					"delete_video"
#define			ACTION_CLEAR_ALL_VIDEO				"clear_all_video"
#define			ACTION_CANCEL_VIDEO					"cancel_gen_video"
#define			ACTION_GET_TOTAL_GEN_VIDEOES		"get_hl_total_videoes"
#define			ACTION_HL_GEN_VIDEOES				"get_hl_gen_videoes"
#define			ACTION_REFRESH_HL_VIDEOES			"refresh_hl_videoes"
#define			ACTION_GET_PLAY_TIP					"get_play_tip"
#define			ACTION_SET_PLAY_TIP					"set_play_tip"
#define			ACTION_GET_PLAY_INFO				"get_play_info"
#define			ACTION_SET_PLAY_INFO				"set_play_info"
#define			ACTION_SELECT_PLAY_LANG				"select_play_lang"
#define			ACTION_GET_LANG_DL_STATUS			"get_lang_dl_status"
#define			ACTION_CHECK_PLAYLANG_DOWNLOAD		"check_playlang_download"
#define			ACTION_NOTIFY_HL_EVENT				"notify_hl_event"
#define			ACTION_NOTIFY_PLAY_EVENT_START		"notify_play_event_start"
#define			ACTION_NOTIFY_PLAY_EVENT_END		"notify_play_event_end"
#define			ACTION_NOTIFY_STYLEDL_PROGRESS		"notify_style_download_progress"
#define			ACTION_NOTIFY_STYLE_LIST			"notify_style_list"
#define			ACTION_SET_STYLE_DOMAIN				"set_style_domain"
#define			ACTION_GET_STYLEDL_LIST				"get_style_list"
#define			ACTION_SYNC_STYLE_DATA				"sync_style_data"
#define			ACTION_DOWNLOAD_LANG_PACKAGE		"download_lang_package"
#define			ACTION_NOTIFY_LANGDL_PROGRESS		"notify_langpackage_dl_progress"
#define			ACTION_GET_MANUAL_RECORD_TIP		"get_manual_record_tip"
#define			ACTION_SET_MANUAL_RECORD_TIP		"set_manual_record_tip"
#define			ACTION_GET_MANUAL_RECORD_INFO		"get_manual_record_info"
#define			ACTION_SET_MANUAL_RECORD_INFO		"set_manual_record_info"
#define			ACTION_GET_RECORD_TYPE				"get_record_type"
#define			ACTION_SET_RECORD_TYPE				"set_record_type"
#define			ACTION_GET_HL_ENABLE				"get_hl_enable"
#define			ACTION_SET_HL_ENABLE				"set_hl_enable"
#define			ACTION_NOTIFY_HLCAPTURE				"notify_hl_capture"

#endif // MACRODEF_H

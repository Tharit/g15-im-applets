#include <time.h>
#include <windows.h>

#ifndef TRILLIAN_PLUGIN_H
#define TRILLIAN_PLUGIN_H

typedef int (*ttkCallback)(int windowID, char *subwindow, char *event, void *data, void *userData);
typedef int (*plugin_function)(char *guid, char *event, void *data);
typedef int (*plugin_function_send)(char *guid, char *event, void *data);
typedef int (*plugin_function_main)(char *event, void *data);
typedef int (*plugin_function_version)();

#define isTrillianError(error_code) (error_code < 0)
#define isTrillianSuccess(error_code) (error_code >= 0)
#define trillianInitialize(structure) do { memset(&structure,0,sizeof(structure)); structure.struct_size = sizeof(structure); } while (0); 
#define trillianListInitialize(structure)  \
	do {\
		memset(&structure,0,sizeof(structure)); structure.struct_size = sizeof(structure); \
		structure.section_id = structure.parent_id = structure.previous_id = structure.unique_id = -1;\
		structure.selectable = 1;\
		trillianInitialize(structure.font); \
		structure.font.flags = 1; \
		structure.font.hover_background = structure.font.hover_foreground = structure.font.select_background = structure.font.select_foreground = structure.font.background = structure.font.foreground = -1; \
	} while (0); 

#define TRILLIAN_PREFS_CANCEL 0
#define TRILLIAN_PREFS_APPLY  1
#define TRILLIAN_PREFS_OK     2

#define TRILLIAN_PREFS_SHOW   1
#define TRILLIAN_PREFS_HIDE   0

#define MENU_TEXT             0  
#define MENU_SEPARATOR        1  
#define MENU_POPUP            2  
#define MENU_HEADING          3  
#define MENU_CALLBACK         4 
#define MENU_DEFAULT          5   
    
#define MENU_TYPE_RIGHTCLICK  0
#define MENU_TYPE_BAR         1

#define MENU_TIMEOUT_DEFAULT  0  
#define MENU_TIMEOUT_INFINITE -1  

#define EVENTS_STATUS_UNKNOWN 0    
#define EVENTS_STATUS_AWAY    1    
#define EVENTS_STATUS_BACK    2   
#define EVENTS_STATUS_IDLE    3   
#define EVENTS_STATUS_HIDDEN  4
#define EVENTS_STATUS_INVISIBLE  5

#define EVENTS_START	0
#define EVENTS_EXECUTE  1
#define EVENTS_END      2

struct plugin_info_t
{
	unsigned int				struct_size;

	/* Information given to the plugin 
	 */

	char						*config_directory;
	char						*skin_directory;
	char						*temp_directory;

	plugin_function_send		plugin_send;

	/* Plugin Fills out this information 
	 */

	char						guid[128];

	char						name[256];
	char						company[256];
	char						version[64];
	char						description[1024];

	/* 2.0 information: given to the plugin
	 */
	char						*global_config_directory;
	char						*trillian_directory;
	char						*language_directory;

	/* 3.0 information: more information for plugin to fill out
	 */
	unsigned char				*png_image_48;
	unsigned int				png_image_48_len;

	unsigned char				*png_image_32;
	unsigned int				png_image_32_len;
};

struct alert_t
{
	unsigned int   				struct_size;

	char						*text;
	char						*type;
	
	int							word_wrap;
	int							formatted;
	int							link;

	ttkCallback 				callback;
	
	void						*data;

	/* 2.0 
	 */
	int							persistent;
};

struct plugin_prefs_info_t
{
	unsigned int   				struct_size;

	char						name[128];
	char						description[1024];

	HBITMAP						bitmap;
};

struct plugin_prefs_show_t
{
	unsigned int	   			struct_size;

	int   						show; //1 shows, 0 hides

	char 						*pref_name;
	char 						*sub_entry;
	
	HWND  						hwnd;

	int   						x;
	int   						y;

	struct plugin_prefs_info_t 	prefs_info;
};

struct plugin_prefs_action_t
{
	unsigned int   				struct_size;

	int 						type;  //0 = cancel, 1 = apply, 2 = ok
};

struct plugin_prefs_entry_t
{
	unsigned int   				struct_size;

	char 						*sub_name;

	struct plugin_prefs_entry_t	*next;
};

struct plugin_prefs_t
{
	unsigned int   				struct_size;

	int 						enabled;

	char 						*pref_name;

	struct plugin_prefs_entry_t	*sub_entry;
};

struct alias_t
{
	unsigned int   				struct_size;

	int     					alias_id;

	char						*text;

	ttkCallback					callback;

	void						*data;
};

struct alias_request_t
{
	unsigned int   				struct_size;

	int							alias_id;

	char						*alias;
	char						*text;

	/* 2.0
	 */
	char						*formatted_text;
	char						*medium;
	int							connection_id;
	int							window_id;
};

struct keyboard_alias_t
{
	unsigned int   				struct_size;

	int							alias_id;

	int							alt;
	int							ctrl;
	int							shift;

	char						key;

	ttkCallback					callback;

	void						*data;
};

struct keyboard_alias_request_t
{
	unsigned int   				struct_size;

	int							alias_id;

	int							alt;
	int							ctrl;
	int							shift;

	char						key;

	char						*text; //to be inserted...if none, make this NULL

	/* 2.0
	 */
	char						*medium;
	int							connection_id;
	int							window_id;
}; 

struct list_bmp_t
{
	unsigned int   				struct_size;

	int							num_states;

	HBITMAP						*bitmap;
	RECT						*location;

	/* Animated support not available currently 
	 */

	int							animated;
	int							time;
	int							loop;
	int							cur_state;

	/* Skin based references 
	 */

	int							*skin_based;  

	char						**skin_name;
	char						**skin_type;
	char						**skin_state;
};

struct list_font_t
{
	unsigned int   				struct_size;

	int							flags;  //0x00000000000000
										//0x00001 = skins
										//0x00010 = font file not the other stuff

	char   						*skin_name;

	HFONT   					font;

	//skin based
	char						*hover_fore;
	char						*hover_back;
	char						*select_fore;
	char						*select_back;
	char						*normal_fore;
	char						*normal_back;

	//nonskin based
	COLORREF  					hover_background;
	COLORREF  					hover_foreground;
	COLORREF  					select_background;
	COLORREF  					select_foreground;
	COLORREF  					background;
	COLORREF  					foreground;

};

struct menu_entry_t
{
	unsigned int   				struct_size;

	int							menu_id;
	int							sub_menu_id;
	int							type;
	
	int							timeout;
	int							disabled;
	int							num_copies;

	char						*icon;
	char						*text;
	char						*shortcut;

	void						*data;
	HWND						hwnd;

	struct menu_entry_t 		*sub_menu;
	struct menu_entry_t 		*next_menu;

	ttkCallback 				callback;
};

struct list_entry_t 
{
	unsigned int   				struct_size;

	int							num_left_icons;		
	struct list_bmp_t			*left_icons;

	int							num_right_icons;		
	struct list_bmp_t			*right_icons;

	struct list_font_t			font;

	int							section_id;
	int							parent_id;
	int							previous_id;
	int							unique_id;

	int							group;				
	int							section;

	int							drag_and_drop;
	int							inline_editing;		
	int							selectable;			
	int							expanded;			
	int							auto_expand;
	int							no_group_icon;
	int							no_double_click;
	int							owner_drawn;
	int							no_section_margins;
	
	int							floating;
	int							floating_x;
	int							floating_y;

	char						*tooltip;			
	char						*text;				
	char						*edit_string;
	char						*real_name;

	void						*data;				
	
	struct menu_entry_t			*menu_entry;

	ttkCallback					callback;

	/* 2.0 
	 */
	int							inline_editing_quick;

	/* 3.0
	 */
	int							icon_mode;
	int							icon_width;
	int							icon_height;

	char						*icon_name;
	char						*panel;
};

struct dialog_entry_t
{
	unsigned int				struct_size;

	HWND						hwnd;
};

struct list_update_t 
{
	unsigned int				struct_size;

	HDC							hdc;
	HWND						hwnd;

	int							x;
	int							y;
	int							y_offset;

	int							width;
	int							height;

	//colors
	COLORREF					hover_background;
	COLORREF					hover_foreground;
	COLORREF					hover_border;
	COLORREF					select_background;	
	COLORREF					select_foreground;
	COLORREF					select_border;
	COLORREF					background;
	COLORREF					foreground;

	//state	
	int							selected;
	int							hover;
	int							floating;
	int							black_and_white;
};

struct connection_enum_t
{
	unsigned int				struct_size;

	char						*medium;

	ttkCallback					callback;

	void						*data;
};

struct connection_entry_t
{
	unsigned int				struct_size;

	char						*medium;

	int							connection_id;

	char						*name;
	char						*status;

	/* 2.0
	 */
	char						*section;
	struct menu_entry_t			*menu_entry;
};

struct contactlist_enum_t
{
	unsigned int				struct_size;

	char						*medium;

	int							connection_id;

	ttkCallback					callback;
	
	void						*data;

	/* 2.0
	 */
	char						*section;
};

struct contactlist_entry_t
{
	unsigned int				struct_size;

	char						*medium;

	int							connection_id;

	char						*name;
	char						*real_name;
	char						*status;

	/* 2.0 enhancement
	 */
	char						*section;
	char						*uri;
	char						*tooltip;

	int							status_score;
	int							status_idle;
	int							status_block;

	struct group_entry_t		*group;
	struct menu_entry_t			*menu_entry;

	ttkCallback					callback;
	void						*data;

	/* 3.0 
	 */
	int							status_warned;
	int							status_time;
	int							status_idle_time;

	char						*status_message;
};

struct message_t
{
	unsigned int				struct_size;

	char						*medium;
	int							connection_id;

	char						*name;
	char						*type;
	char						*text;

	char						*location;

	void						*extra_information;
	
	/* 2.0 
	 */
	int							window_id;
	unsigned int				extra_information_size;
	unsigned int				time_stamp;

	char						*display_name;
	char						*section;

	/* 3.0
	 */
	struct capability_t			*capability_list;
	int							text_len;
	int							message_id;
};

struct message_broadcast_t
{
	unsigned int				struct_size;

	int							broadcast_id;

	ttkCallback					callback;

	void						*data;
};

/* 2.0 structures
 */

struct medium_entry_t
{
	unsigned int				struct_size;

	char						*medium;
	char						*description;

	int							allows_accounts;
	int							allows_connections;
	int							allows_contacts;
	int							allows_contact_importing; 
	int							allows_message_sending;
	int							allows_message_receiving;
	int							allows_file_sending;
	int							allows_file_receiving;
	int							allows_massmessaging;

	ttkCallback					callback;
	
	void						*data;

	/* 3.0
	 */
	int							allows_account_editing;
	int							allows_account_removing;
	int							allows_contact_adding;

	unsigned char				*png_image_32;
	unsigned int				png_image_32_len;
	
	unsigned char				*png_image_16;
	unsigned int				png_image_16_len;
};

struct group_entry_t
{
	unsigned int				struct_size;

	char						*name;

	struct group_entry_t		*child;

	/* 3.0
	 */
	int							is_metacontact;
};

struct skin_entry_t
{
	unsigned int				struct_size;

	char						*xml_text;
};

struct nicklist_change_t
{
	unsigned int				struct_size;

	int							window_id;
	int							connection_id;

	char						*medium;
	char						*location;
	
	struct nicklist_entry_t*	nicklist;
};

struct nicklist_entry_t
{
	unsigned int				struct_size;

	char						*name;
	char						*group;
	char						*tooltip;

	int							group_score;

	struct menu_entry_t			*menu;

	ttkCallback					callback;

	void						*data;

	struct nicklist_entry_t*	next_nicklist_entry;	
};

struct message_options_t
{
	unsigned int				struct_size;

	int							global_mode;

	int							bold;
	int							italics;
	int							underline;
								
	int							style;
	int							size;
								
	int							background;
	int							foreground;
	int							num_colors;
								
	int							images;
	int							images_status;
								
	int							max_length;
	int							disabled;
	char						*disable_message;
								
	int							logging_off;
	char						*logging_name;

	char						*echo_name;
	
	int							echo_off;
	int							is_contact;
	int							initiated;
	
	int							irc_style;

	struct nicklist_entry_t		*nicklist;

	/* 3.0
	 */

	int							video;
	int							audio;
};

struct message_toolbar_action_t
{
	unsigned int				struct_size;

	int							connection_id;
	int							window_id;

	char						*medium;
	char						*name;
	char						*location;

	char						*action;

	/* 3.0
	 */
	HWND						hwnd;
};

struct message_toolbar_register_t
{
	unsigned int				struct_size;

	int							connection_id;
	int							window_id;

	char						*medium;
	char						*name;
	char						*location;

	char						*button;
	char						*tooltip;
	int							toolbar_id;

	ttkCallback					callback;
	void						*data;
};


struct message_menu_t
{
	unsigned int				struct_size;

	int							connection_id;
	int							window_id;

	char						*medium;
	char						*name;
	char						*location;

	int							menu_type;

	char						*menu_name;
	struct menu_entry_t			*menu_entry;
};

struct message_state_t
{
	unsigned int				struct_size;

	int							connection_id;
	int							window_id;

	char						*medium;
	char						*name;
	char						*location;

	char						*control;
	char						*state;
};

#define IMAGE_ALL				0x0000
#define IMAGE_PNG				0x0001
#define IMAGE_GIF				0x0002
#define IMAGE_JPG				0x0004
#define IMAGE_JP2				0x0008
#define IMAGE_ICO				0x0010
#define IMAGE_BMP				0x0020

struct avatar_t
{
	unsigned int				struct_size;

	int							connection_id;
	int							window_id;
	int							timestamp;
	int							data_length;

	char						*medium;
	char						*name;
	char						*filename;
	char						*data;

	/* 3.0
	 */
	int							force_file_size;
	int							force_image_dimensions;
	int							allowed_image_types;

	int							image_width;
	int							image_height;
	
	int							file_size;

	/* 3.1
	 */
	char						*section;
};

struct event_action_register_t
{
	unsigned int				struct_size;

	char						*event;
	char						*description;
	
	int							custom_settings;
	int							event_id;

	ttkCallback					callback;
	void						*data;
};

struct event_action_edit_t
{
	unsigned int				struct_size;

	char						*event;

	void						*action_data;

	int							event_id;
};

struct event_action_show_t
{
	unsigned int				struct_size;  	
	
	int   						show; //1 shows, 0 hides  	
	
	char		 				*event; 	
	
	void						*action_data;

	HWND  						hwnd;  	
	
	int   						x; 	
	int   						y;  	

	int							event_id;
};

struct event_action_attribute_t
{
	unsigned int				struct_size;

	char						*name;
	char						*value;

	void						*action_data;
};

struct event_status_register_t
{
	unsigned int				struct_size;

	char						*medium;
	char						*status;
	char						*description;

	int							default_status;
	int							event_id;

	ttkCallback					callback;
	void						*data;

	/* 3.0
	 */

	unsigned char				*png_image;
	unsigned int				png_image_len;
};

struct event_status_t
{
	unsigned int				struct_size;
	
	char						*medium;
	char						*status;
	char						*text;

	int							auto_respond;
	int							automatic;
};

struct event_variables_t
{
	unsigned int				struct_size;
								
	char						*variable_name;
	char						*variable_type; //string, integer, float, callback, unknown/custom
								
	void						*variable_data;
	unsigned int				variable_size;
								
	struct event_variables_t	*next_evt;
};

struct event_action_execute_t
{
	unsigned int				struct_size;

	struct event_variables_t	*variables;

	void						*action_data;
};

struct event_event_register_t
{
	unsigned int				struct_size;

	char						*type;
	char						*description;
};

struct event_connect_t
{
	unsigned int				struct_size;
							
	char						*type;
							
	int							event_id;
							
	ttkCallback					callback;
	void						*data;
};

struct event_t
{
	unsigned int				struct_size;
								
	char						*type;
	char						*description;
								
	int							discrete;
	int							event_id;
	
	struct event_variables_t	*variables;
};


struct browser_location_t
{
	unsigned int				struct_size;

	char						*location;

	int							new_window;
	int							check_security;
};

struct browser_entry_t
{
	unsigned int				struct_size;

	char						*protocol;

	char						*name;
	char						*description;

	ttkCallback					callback;
	void						*data;
};

struct account_entry_t
{
	unsigned int				struct_size;

	char						*account;
	char						*medium;
	char						*status;
	char						*section;

	int							connected;
	long						last_connected;

	HICON						icon;
	HWND						window;

	/* 3.0
	 */
	ttkCallback					callback;
	void						*data;
};

struct contact_wizard_info_t
{
	unsigned int   				struct_size;

	char						name[128];
	char						description[1024];

	HBITMAP						bitmap;
};

#define CONTACT_WIZARD_OVERRIDE_GROUPS	0x0001

struct contact_wizard_t
{
	unsigned int	   			struct_size;

	int   						show; //1 shows, 0 hides
	int							flags;

	char 						*medium;
	char 						*type; //contact, import
	
	HWND  						hwnd;

	int   						x;
	int   						y;

	char						*section;
	struct group_entry_t		*group;

	struct contact_wizard_info_t 	wizard_info;
};

struct filetransfer_update_t
{
	unsigned int				struct_size;

	char						*medium;

	int							connection_id;
	int							filetransfer_id;

	unsigned int				file_size;
	unsigned int				start_size;

	char						*local_screen_name;
	char						*remote_screen_name;
	char						*filename;
};

struct filetransfer_init_t
{
	unsigned int				struct_size;

	char						*medium;

	int							connection_id;
	int							filetransfer_id;

	/* status of the transfer */
	int							accepted;
	int							resume;

	/* preferences */
	int							incoming;
	int							ask_resume;
	int							ask_location;
	int							ask_screen_name;
	int							ask_medium;
	int							ask_description;
	int							ask_confirmation;
							
	int							in_contact_list; 

	unsigned int				file_size;
	unsigned int				start_size; /* for resuming */

	char						*local_screen_name;
	char						*remote_screen_name;
							
	char						*filename;
	char						*description;
	
	ttkCallback					callback;

	void						*data;
};

struct filetransfer_request_t
{
	unsigned int				struct_size;

	char						*medium;

	int							connection_id;
	int							filetransfer_id;

	int							ask_description;

	/* 3.0
	 */
	int							window_id;
	char						*remote_screen_name;
};

struct filetransfer_status_t
{
	unsigned int				struct_size;

	char						*medium;

	int							connection_id;
	int							filetransfer_id;

	char						*type; //error, complete, progress
	char						*description;

	int							bytes;
};

struct edit_event_t
{
	unsigned int   				struct_size;

	int							window_id;
	int     					edit_event_id;

	char						*event;

	ttkCallback					callback;

	void						*data;
};

struct edit_menu_t
{
	unsigned int   				struct_size;

	int							menu_id;

	struct menu_entry_t			*menu_entry;
};

struct xml_attribute_t
{
	unsigned int				struct_size;

	char						*name;
	char						*value;

	struct xml_attribute_t		*next_attribute;

	/* 3.0
	 */
	struct xml_attribute_t		*previous;
};

struct xml_tag_t
{
	unsigned int				struct_size;

	struct xml_attribute_t		*attributes;

	struct xml_tag_t			*children;

	char						*text;
	char						*type; // "tag", "text"

	struct xml_tag_t			*next_tag;

	/* 3.0
	 */
	struct xml_tag_t			*parent;
	struct xml_tag_t			*previous;
};

struct xml_tree_t
{
	unsigned int				struct_size;

	int							memory_id;

	char						*filename;
	char						*data;

	struct xml_tag_t			*root_tag;
};

struct xml_string_t
{
	unsigned int				struct_size;

	char						*string_buffer;
	
	struct xml_tree_t			*xml_tree;
};

struct plugin_external_t
{
	unsigned int				struct_size;
	
	char						*guid;

	char						*event;
	void						*data;
};

struct plugin_external_enum_t
{
	unsigned int				struct_size;

	char						*type;

	ttkCallback					callback;
	void						*data;
};

struct connection_bytes_t
{
	unsigned int				struct_size;

	char						*medium;

	int							connection_id;

	int							sent_bytes_change;
	int							recv_bytes_change;
};

struct skin_location_t
{
	unsigned int				struct_size;

	int							num;

	char						*orientation;
	char						*type;
	char						*source;
};

struct skin_rect_t
{
	unsigned int				struct_size;

	struct skin_location_t		*left;
	struct skin_location_t		*top;
	struct skin_location_t		*right;
	struct skin_location_t		*bottom;
};

struct skin_taskbaricon_t
{
	unsigned int				struct_size;

	char						*name;
	char						*file;
};

struct skin_bitmap_t
{
	unsigned int				struct_size;

	char						*name;
	char						*file;
};

struct skin_font_t
{
	unsigned int				struct_size;

	char						*name;
	char						*file;
};

struct skin_sound_t
{
	unsigned int				struct_size;

	char						*name;
	char						*file;
};

struct skin_window_t
{
	unsigned int				struct_size;

	char						*type;
	char						*name;
	char						*source;
	char						*phase;
	char						*taskbar;
	char						*container_taskbar;
};

struct skin_control_t
{
	unsigned int				struct_size;

	char						*name;
	char						*type;
};

struct skin_component_t
{
	unsigned int				struct_size;

	char						*name;

	int							minx;
	int							maxx;
	int							miny;
	int							maxy;
};

struct skin_icontrol_t
{
	unsigned int				struct_size;

	char						*source;
	char						*name;

	int							visible;
};

struct skin_frame_t
{
	unsigned int				struct_size;

	char						*source;
	char						*name;
};

struct skin_drawer_t
{
	unsigned int				struct_size;

	char						*source;
	char						*name;
	char						*attach;

	int							visible;
};

struct skin_source_t
{
	unsigned int				struct_size;

	char						*name;

	int							left;
	int							top;
	int							right;
	int							bottom;
};

struct skin_control_entry_t
{
	unsigned int				struct_size;

	struct skin_control_t		*control;

	char						*tag;
	void						*tag_data;
};

struct skin_control_font_t
{
	unsigned int				struct_size;

	char						*source;
	char						*type;
	char						*name;

	int							size;
	int							bold;
	int							italics;
	int							underline;
};

struct skin_control_forecolor_t
{
	unsigned int				struct_size;

	int							red;
	int							green;
	int							blue;
};

struct skin_control_backcolor_t
{
	unsigned int				struct_size;

	int							red;
	int							green;
	int							blue;
};

struct skin_control_setting_t
{
	unsigned int				struct_size;

	char						*name;
	char						*value;
};

struct skin_control_color_t
{
	unsigned int				struct_size;

	char						*name;

	int							red;
	int							green;
	int							blue;

	struct skin_rect_t			*rect;
};

struct skin_control_colors_t
{
	unsigned int				struct_size;

	char						*file;
};

struct skin_control_action_t
{
	unsigned int				struct_size;

	char						*name;
	char						*value;
};

struct skin_control_tooltip_t 
{
	unsigned int				struct_size;

	char						*text;
};

struct skin_enum_t
{
	unsigned int				struct_size;

	char						*search_type;
	void						*search_data;

	ttkCallback					callback;
	
	void						*data;
};

struct skin_control_icon_t
{
	unsigned int				struct_size;

	char						*name;
	char						*type;
	char						*state;

	struct skin_source_t		*source;

};

struct skin_control_background_t
{
	unsigned int				struct_size;

	char						*name;
	char						*type;

	int							transparent;

	int							transred;
	int							transgreen;
	int							transblue;
	int							thresred;
	int							thresgreen;
	int							thresblue;

	struct skin_source_t		*source;
	struct skin_rect_t			*rect;
};

struct skin_control_emoticon_t
{
	unsigned int				struct_size;

	char						*text;
	char						*sound;
	char						*display;
	char						*medium;

	int							button;

	struct skin_source_t		*source;

};

struct skin_control_icontrol_t
{
	unsigned int				struct_size;

	char						*source;
	char						*name;

	int							visible;

	struct skin_rect_t			*rect;
};


struct contactlist_subcontact_t
{
	unsigned int				struct_size;

	struct contactlist_entry_t	*parent;
	struct contactlist_entry_t	*subcontact;
};

struct tooltip_item_t
{
	unsigned int				struct_size;

	char						*type; // can be either text, or separator

	char						*name;
	char						*value;

	struct tooltip_item_t		*next_item;
};

struct tooltip_entry_t
{
	unsigned int				struct_size;

	char						*title;
	char						*sub_title;
	char						*icon;

	struct tooltip_item_t		*item_list;
};

struct tooltip_request_t
{
	unsigned int				struct_size;

	int							id;

	ttkCallback					callback;
	void						*data;
};

struct contactlist_tooltip_request_t
{
	unsigned int				struct_size;

	struct contactlist_entry_t  *contact;

	ttkCallback					callback;
	void						*data;
};

struct contactlist_tooltip_register_t
{
	unsigned int				struct_size;

	int							tooltip_id;

	struct contactlist_entry_t	*contact;

	ttkCallback					callback;
	void						*data;

};

struct contactlist_group_change_t
{
	unsigned int				struct_size;

	struct contactlist_entry_t	*contact;

	struct group_entry_t		*old_group;
	struct group_entry_t		*new_group;
};

struct message_morph_t
{
	unsigned int				struct_size;

	struct message_t			*original_window;
	struct message_t			*new_window;
};

#define HTTP_FLAG_SECURE		0x02 
#define HTTP_PROXY_SOCKS4		1 
#define HTTP_PROXY_SOCKS5		2 
#define HTTP_PROXY_HTTP			3 

struct http_request_t 
{ 
	unsigned int				struct_size; 

	char						*url; 

	ttkCallback					callback; 
	void						*data; 

	int							content_length; 

	char						*content_type; 
	char						*content_data; 

	/* 3.1 
	 */ 

	char						*command; 
	unsigned int				flags; 

	unsigned char				*http_header; 
	int							http_header_len; 

	int							pi_use; 
	int							pi_type; 
	int							pi_use_authentication; 

	char						*pi_server; 
	char						*pi_port; 
	char						*pi_username; 
	char						*pi_password; 
}; 

#define HTTP_RESULT_CONNECTIONFAIL		0 
#define HTTP_RESULT_CONNECTIONSUCCEED	1 
#define HTTP_ERROR_INVALID_URL			0 

struct http_result_t 
{ 
	unsigned int				struct_size; 

	char						*url; 

	char						*buffer; 
	int							buffer_size; 

	/* 3.1 
	 */ 

	int							result_state; 
	int							connection_error_code; 
	int							http_response_code; 
	char						*http_response_headers; 
}; 

struct language_entry_t
{
	unsigned int				struct_size;

	char						*language_directory;
};


/* 2.01 structures
 */

#define INSTALL_WIZARD_FORWARD	0x0001
#define INSTALL_WIZARD_BACKWARD	0x0002

struct install_wizard_t
{
	unsigned int				struct_size;

	int   						show; //1 shows, 0 hides
	int							flags;

	HWND  						hwnd;

	int   						x;
	int   						y;

	/* 3.0
	 */
	char						name[128];
	char						description[1024];
};

/* 3.0 structures
 */

#define AUDIO_CODEC_UNCOMPRESSED	0 
#define AUDIO_CODEC_TRUESPEECH		1 
 
struct audio_t 
{ 
    unsigned int				struct_size; 

	int							connection_id;
	int							window_id;

	char						*medium;
	char						*name;
	char						*location;

	char						*section;

	int							initiated;
	int							ask_confirmation;

	/* Audio Information
	 */

    int							codec; 

	int							audio_sample_size;
	int							audio_sample_rate;
	int							audio_block_size;
	int							audio_quality;

    int							audio_data_length; 

    unsigned char				*audio_data; 

    ttkCallback					callback; 
    void						*data; 
};

struct audio_status_t
{
	unsigned int				struct_size;

	int							connection_id;
	int							window_id;

	char						*medium;
	char						*name;
	char						*location;

	char						*type;
	char						*status;

	char						*link;
	char						*tooltip;
};

#define VIDEO_CODEC_UNCOMPRESSED	0 
#define VIDEO_CODEC_MJPG			1 

struct video_t 
{ 
	unsigned int				struct_size; 

	int							connection_id;
	int							window_id;

	char						*medium;
	char						*name;
	char						*location;

	char						*section;

	int							initiated;
	int							ask_confirmation;

	/* Video Information
	 */

	int							codec; 

	int							video_width; 
	int							video_height; 
	int							video_depth;
	int							video_quality;
	int							video_fps;

	int							video_data_length; 

	unsigned char				*video_data; 

	ttkCallback					callback; 
	void						*data; 
};

struct video_status_t
{
	unsigned int				struct_size;

	int							connection_id;
	int							window_id;

	char						*medium;
	char						*name;
	char						*location;

	char						*type;
	char						*status;

	char						*link;
	char						*tooltip;
};

#define MESSAGE_SEARCH_NAME		0x0001
#define MESSAGE_SEARCH_MESSAGE	0x0002
#define MESSAGE_SEARCH_TIME		0x0004
#define MESSAGE_SEARCH_TYPE		0x0008
#define MESSAGE_SEARCH_MEDIUM	0x0010
#define MESSAGE_SEARCH_LIMITED	0x0020
#define MESSAGE_SEARCH_LOCATION	0x0040
#define MESSAGE_SEARCH_SLOW		0x0080

//spaces separate words.  so if you want to search for anything with bob and tom in it, search for "bob tom"
struct message_search_t
{
	unsigned int				struct_size;

	int							search_id;

	int							search_flags; 

	int							case_sensitive;

	char						*type; 		//message / video /conversation

	char						*message_type; //information_standard , etc

	int							name_fullmatch;
	char						*name;			//depending on the type of message, the name field may match different things..ie: status changes, name will be the 'who' field, but otherwise it could be the to or the from field.
	char						*location;

	int							message_fullmatch;
	int							message_containing_links;
	int							message_fullwords;
	char						*message;

	char						*medium;

	unsigned long				time_start;
	unsigned long				time_end;

	ttkCallback					callback;
	void						*data;
};

struct message_search_entry_t
{
	unsigned int				struct_size;
	
	char						*to;
	char						*from;
	char						*from_display;
	char						*text;
	char						*link;
	char						*screenshot;
	char						*location;

	char						*type;

	char						*message_type;
	char						*medium;

	time_t						timestamp;
};

struct progress_t
{
	unsigned int				struct_size;

	unsigned int				position;
	unsigned int				total;
};

struct capability_t
{
	unsigned int				struct_size;

	char						*capability;

	struct capability_t			*next_capability;
};

struct account_interface_entry_t
{
	unsigned int				struct_size;

	struct account_entry_t		*account;

	char						*name;

	char						*type;
	char						*description;
	char						*value;

	struct account_interface_entry_t	*next_entry;
};

struct account_interface_request_t 
{ 
	unsigned int				struct_size; 
 
	struct account_entry_t		*account; 
 
	ttkCallback					callback; 
	void						*data; 
};

struct plugin_file_t
{
	unsigned int				struct_size;

	char						*filename;

	struct plugin_file_t		*next_file;
};

struct plugin_prefs_value_register_t
{
	unsigned int				struct_size;

	char						*value;

	ttkCallback					callback;
	void						*data;

	/*
	 */
	int							watcher;
};

struct plugin_prefs_value_request_t 
{
	unsigned int				struct_size;

	char*						value;

	struct event_variables_t	*parameters;

	ttkCallback					callback;
	void						*data;
};

struct plugin_prefs_value_t 
{
	unsigned int				struct_size;

	char*						value;

	struct event_variables_t	*parameters;
};

struct awaymessage_enum_t
{
	unsigned int				struct_size;

	ttkCallback					callback;
	void						*data;
};

struct awaymessage_entry_t
{
	unsigned int				struct_size;

	char						*name;
};
#endif



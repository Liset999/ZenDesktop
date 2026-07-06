import os
import sys
import ctypes
import winreg
import customtkinter as ctk

# Ensure Admin Privileges for Registry Writing
def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False

if not is_admin():
    print("Requesting Admin Privileges...")
    # Re-launch the script with admin privileges
    ctypes.windll.shell32.ShellExecuteW(
        None, "runas", sys.executable, f'"{__file__}"', None, 1
    )
    sys.exit(0)

# Window Configuration
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

class ZenCustomizerApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        
        self.title("ZenDesktop 实时调色与个性化面板 (CTM)")
        self.geometry("560x820")
        self.resizable(False, False)
        
        # Registry Paths for local mods
        self.mods = {
            "Taskbar": r"SOFTWARE\Windhawk\Engine\Mods\local@zen-taskbar-acrylic\Settings",
            "StartMenu": r"SOFTWARE\Windhawk\Engine\Mods\local@zen-startmenu-acrylic\Settings",
            "NotificationCenter": r"SOFTWARE\Windhawk\Engine\Mods\local@zen-notificationcenter-acrylic\Settings",
            "StageManager": r"SOFTWARE\Windhawk\Engine\Mods\local@zen-stage-manager\Settings"
        }

        self.setting_keys = {
            "Taskbar": {
                "color": "bgColorMode",
                "blur": "blurPreset",
                "opacity": "opacityPreset",
                "luminosity": "luminosityPreset",
            },
            "StartMenu": {
                "color": "bgColorMode",
                "blur": "blurPreset",
                "opacity": "opacityPreset",
                "luminosity": "luminosityPreset",
            },
            "NotificationCenter": {
                "color": "bgColorMode",
                "blur": "blurPreset",
                "opacity": "opacityPreset",
                "luminosity": "luminosityPreset",
            }
        }

        self.setting_defaults = {
            "Taskbar": {
                "color": "Default",
                "blur": 30,
                "opacity": 50,
                "luminosity": 100,
            },
            "StartMenu": {
                "color": "Default",
                "blur": 30,
                "opacity": 50,
                "luminosity": 100,
            },
            "NotificationCenter": {
                "color": "Default",
                "blur": 30,
                "opacity": 50,
                "luminosity": 100,
            }
        }
        
        # Color Options Mapping (English values in settings -> Chinese display names)
        self.color_options_cn_to_en = {
            "主题默认": "Default",
            "跟随系统强调色": "Accent",
            "晶莹透白 (Glass White)": "GlassWhite",
            "迷雾墨灰 (Mist Grey)": "MistGrey",
            "深空极黑 (Space Black)": "DeepBlack",
            "深海湛蓝 (Ocean Blue)": "OceanBlue",
            "极光幻青 (Aurora Cyan)": "AuroraCyan",
            "玫瑰幽粉 (Rose Pink)": "RosePink",
            "波尔多红 (Bordeaux Red)": "BordeauxRed",
            "森林黛绿 (Forest Green)": "ForestGreen",
            "皇家黛紫 (Royal Purple)": "RoyalPurple",
            "落日熔橙 (Sunset Orange)": "SunsetOrange",
            "香槟金黄 (Champagne Gold)": "ChampagneGold",
            "莫兰迪绿 (Morandi Sage)": "MorandiSage"
        }
        self.color_options_en_to_cn = {v: k for k, v in self.color_options_cn_to_en.items()}

        self.create_widgets()
        self.load_all_settings()
        
    def create_widgets(self):
        # Premium Title Label with modern styling
        title_frame = ctk.CTkFrame(self, fg_color="transparent")
        title_frame.pack(pady=20, fill="x", padx=20)
        
        title_label = ctk.CTkLabel(
            title_frame, 
            text="ZenDesktop 视觉控制中心", 
            font=ctk.CTkFont(family="Segoe UI", size=24, weight="bold"),
            text_color="#00D2FF"
        )
        title_label.pack()
        
        subtitle_label = ctk.CTkLabel(
            title_frame, 
            text="无极高阶调色 • 实时预览生效 • 极致稳定架构", 
            font=ctk.CTkFont(family="Microsoft YaHei", size=13),
            text_color="#888888"
        )
        subtitle_label.pack(pady=2)

        # Tabview for Taskbar, Start Menu, Notification Center, File Explorer
        self.tabview = ctk.CTkTabview(
            self, 
            width=500, 
            height=540, 
            segmented_button_selected_color="#00D2FF", 
            segmented_button_selected_hover_color="#00B2DF",
            segmented_button_unselected_color="#222222",
            segmented_button_unselected_hover_color="#333333"
        )
        self.tabview.pack(pady=10, fill="both", expand=True, padx=20)
        
        self.tabs = {}
        for name in ["Taskbar", "StartMenu", "NotificationCenter", "StageManager", "FileExplorer"]:
            tab_title = {
                "Taskbar": "任务栏 (Taskbar)",
                "StartMenu": "开始菜单 (Start Menu)",
                "NotificationCenter": "通知中心 (Notification Center)",
                "StageManager": "台前调度 (Stage Manager)",
                "FileExplorer": "资源管理器 (Explorer)"
            }[name]
            self.tabs[name] = self.tabview.add(tab_title)
            if name == "FileExplorer":
                self.build_file_explorer_tab(self.tabs[name])
            elif name == "StageManager":
                self.build_stage_manager_tab(self.tabs[name])
            else:
                self.build_tab(name, self.tabs[name])
            
        # Unified Controls Frame at bottom
        bottom_frame = ctk.CTkFrame(self, fg_color="transparent")
        bottom_frame.pack(pady=15, fill="x", padx=20)
        
        sync_btn = ctk.CTkButton(
            bottom_frame,
            text="一键同步玻璃参数 (将当前任务栏设置应用到所有组件)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            fg_color="#00D2FF",
            hover_color="#00B2DF",
            text_color="#111111",
            height=38,
            command=self.sync_all_mods
        )
        sync_btn.pack(fill="x")

        restart_btn = ctk.CTkButton(
            bottom_frame,
            text="应用并立即刷新桌面环境 (Refresh Shell)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            fg_color="#00FF66",
            hover_color="#00DD55",
            text_color="#111111",
            height=38,
            command=self.restart_shell
        )
        restart_btn.pack(fill="x", pady=(8, 0))
        
    def build_select_row(self, parent_tab, label_text, values, command):
        frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        frame.pack(fill="x", pady=10, padx=15)

        label = ctk.CTkLabel(
            frame,
            text=label_text,
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        label.pack(fill="x")

        dropdown = ctk.CTkOptionMenu(
            frame,
            values=values,
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            dropdown_font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            fg_color="#2D2D2D",
            button_color="#3D3D3D",
            button_hover_color="#4D4D4D",
            height=32,
            command=command
        )
        dropdown.pack(fill="x", pady=6)
        return dropdown

    def build_file_explorer_tab(self, parent_tab):
        # Title/Description for ExplorerBlurMica
        info_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        info_frame.pack(fill="x", pady=15, padx=20)

        lbl_title = ctk.CTkLabel(
            info_frame,
            text="文件资源管理器透明 (ExplorerBlurMica)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=15, weight="bold"),
            text_color="#00D2FF",
            anchor="w"
        )
        lbl_title.pack(fill="x", pady=2)

        lbl_author = ctk.CTkLabel(
            info_frame,
            text="原作者: Maplespe (GitHub: Maplespe/ExplorerBlurMica)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            text_color="#AAAAAA",
            anchor="w"
        )
        lbl_author.pack(fill="x", pady=2)

        lbl_desc = ctk.CTkLabel(
            info_frame,
            text="集成外部优秀的资源管理器背景透明/模糊增强程序。\n注意：此程序非本套件原创，本套件仅提供前端图形化快捷操作入口。",
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            text_color="#CCCCCC",
            justify="left",
            anchor="w"
        )
        lbl_desc.pack(fill="x", pady=6)

        # Divider
        divider = ctk.CTkFrame(parent_tab, height=2, fg_color="#333333")
        divider.pack(fill="x", pady=10, padx=20)

        # Buttons Frame
        btn_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        btn_frame.pack(fill="x", pady=15, padx=20)

        # Button 1: Register / Enable
        btn_register = ctk.CTkButton(
            btn_frame,
            text="注册/启用透明效果 (Register & Enable)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            fg_color="#00D2FF",
            hover_color="#00B2DF",
            text_color="#000000",
            height=40,
            command=self.on_register_explorer_blur_mica
        )
        btn_register.pack(fill="x", pady=8)

        # Button 2: Open Advanced Config Editor
        btn_config = ctk.CTkButton(
            btn_frame,
            text="打开高级透明配置 (Configure ExplorerBlurMica)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            fg_color="#2A2A2A",
            hover_color="#3A3A3A",
            border_width=1,
            border_color="#555555",
            height=40,
            command=self.on_open_explorer_blur_mica_config
        )
        btn_config.pack(fill="x", pady=8)

        # Button 3: Uninstall / Disable
        btn_uninstall = ctk.CTkButton(
            btn_frame,
            text="注销/停用透明效果 (Uninstall & Disable)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13),
            fg_color="#A31C1C",
            hover_color="#C22727",
            height=35,
            command=self.on_uninstall_explorer_blur_mica
        )
        btn_uninstall.pack(fill="x", pady=15)

    def build_stage_manager_tab(self, parent_tab):
        info_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        info_frame.pack(fill="x", pady=15, padx=20)

        lbl_title = ctk.CTkLabel(
            info_frame,
            text="台前调度 (Stage Manager)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=15, weight="bold"),
            text_color="#00D2FF",
            anchor="w"
        )
        lbl_title.pack(fill="x", pady=2)

        lbl_desc = ctk.CTkLabel(
            info_frame,
            text="macOS 风格的窗口工作台管理，自动按应用分组，侧边栏实时预览，一键切换工作区。",
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            text_color="#CCCCCC",
            justify="left",
            anchor="w"
        )
        lbl_desc.pack(fill="x", pady=6)

        divider = ctk.CTkFrame(parent_tab, height=2, fg_color="#333333")
        divider.pack(fill="x", pady=10, padx=20)

        edge_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        edge_frame.pack(fill="x", pady=10, padx=15)

        lbl_edge = ctk.CTkLabel(
            edge_frame,
            text="侧边栏停靠位置 (Sidebar Position):",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        lbl_edge.pack(fill="x")

        self.dropdown_StageManager_edge = ctk.CTkOptionMenu(
            edge_frame,
            values=["左侧 (Left)", "右侧 (Right)"],
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            dropdown_font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            fg_color="#2D2D2D",
            button_color="#3D3D3D",
            button_hover_color="#4D4D4D",
            height=32,
            command=self.on_sm_edge_changed
        )
        self.dropdown_StageManager_edge.pack(fill="x", pady=6)

        width_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        width_frame.pack(fill="x", pady=10, padx=15)

        lbl_width = ctk.CTkLabel(
            width_frame,
            text="侧边栏宽度 (Sidebar Width):",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        lbl_width.pack(fill="x")

        self.slider_StageManager_width = ctk.CTkSlider(
            width_frame,
            from_=60,
            to=160,
            number_of_steps=100,
            fg_color="#333333",
            progress_color="#00D2FF",
            button_color="#00D2FF",
            button_hover_color="#00B2DF",
            command=lambda v: self.on_sm_width_slider(v)
        )
        self.slider_StageManager_width.pack(fill="x", pady=6)

        self.lbl_sm_width_val = ctk.CTkLabel(
            width_frame,
            text="80 px",
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            text_color="#AAAAAA"
        )
        self.lbl_sm_width_val.pack(anchor="e")

        opacity_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        opacity_frame.pack(fill="x", pady=10, padx=15)

        lbl_opacity = ctk.CTkLabel(
            opacity_frame,
            text="亚克力透明度 (Acrylic Opacity):",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        lbl_opacity.pack(fill="x")

        self.slider_StageManager_opacity = ctk.CTkSlider(
            opacity_frame,
            from_=0,
            to=255,
            number_of_steps=255,
            fg_color="#333333",
            progress_color="#00D2FF",
            button_color="#00D2FF",
            button_hover_color="#00B2DF",
            command=lambda v: self.on_sm_opacity_slider(v)
        )
        self.slider_StageManager_opacity.pack(fill="x", pady=6)

        self.lbl_sm_opacity_val = ctk.CTkLabel(
            opacity_frame,
            text="180 / 255",
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            text_color="#AAAAAA"
        )
        self.lbl_sm_opacity_val.pack(anchor="e")

        stages_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        stages_frame.pack(fill="x", pady=10, padx=15)

        lbl_stages = ctk.CTkLabel(
            stages_frame,
            text="最多显示舞台数 (Max Visible Stages):",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        lbl_stages.pack(fill="x")

        self.slider_StageManager_maxstages = ctk.CTkSlider(
            stages_frame,
            from_=1,
            to=10,
            number_of_steps=9,
            fg_color="#333333",
            progress_color="#00D2FF",
            button_color="#00D2FF",
            button_hover_color="#00B2DF",
            command=lambda v: self.on_sm_maxstages_slider(v)
        )
        self.slider_StageManager_maxstages.pack(fill="x", pady=6)

        self.lbl_sm_maxstages_val = ctk.CTkLabel(
            stages_frame,
            text="5",
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            text_color="#AAAAAA"
        )
        self.lbl_sm_maxstages_val.pack(anchor="e")

        toggle_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        toggle_frame.pack(fill="x", pady=10, padx=15)

        lbl_hotkey = ctk.CTkLabel(
            toggle_frame,
            text="Win+G 合并热键 (Grouping Hotkey):",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        lbl_hotkey.pack(fill="x")

        self.switch_StageManager_hotkey = ctk.CTkSwitch(
            toggle_frame,
            text="启用 (Enabled)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            fg_color="#00D2FF",
            progress_color="#00D2FF",
            button_color="#FFFFFF",
            button_hover_color="#EEEEEE",
            command=self.on_sm_hotkey_toggle
        )
        self.switch_StageManager_hotkey.pack(anchor="w", pady=6)

        anim_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        anim_frame.pack(fill="x", pady=10, padx=15)

        lbl_anim = ctk.CTkLabel(
            anim_frame,
            text="切换时禁用窗口动画 (Disable Animations):",
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        lbl_anim.pack(fill="x")

        self.switch_StageManager_anim = ctk.CTkSwitch(
            anim_frame,
            text="启用 (Enabled)",
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            fg_color="#00D2FF",
            progress_color="#00D2FF",
            button_color="#FFFFFF",
            button_hover_color="#EEEEEE",
            command=self.on_sm_anim_toggle
        )
        self.switch_StageManager_anim.pack(anchor="w", pady=6)

    def build_tab(self, mod_key, parent_tab):
        # 1. Background Color Preset Dropdown
        color_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        color_frame.pack(fill="x", pady=12, padx=15)
        
        lbl_color = ctk.CTkLabel(
            color_frame, 
            text="背景颜色预设 (Color Preset):", 
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        lbl_color.pack(fill="x")
        
        dropdown = ctk.CTkOptionMenu(
            color_frame,
            values=list(self.color_options_cn_to_en.keys()),
            font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            dropdown_font=ctk.CTkFont(family="Microsoft YaHei", size=12),
            fg_color="#2D2D2D",
            button_color="#3D3D3D",
            button_hover_color="#4D4D4D",
            height=32,
            command=lambda val, k=mod_key: self.on_color_preset_changed(k, val)
        )
        dropdown.pack(fill="x", pady=6)
        setattr(self, f"dropdown_{mod_key}", dropdown)
        
        # Divider Line
        divider = ctk.CTkFrame(parent_tab, height=2, fg_color="#333333")
        divider.pack(fill="x", pady=10, padx=15)
        
        # 2. Blur Slider
        if mod_key != "FileExplorer":
            blur_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
            blur_frame.pack(fill="x", pady=10, padx=15)
            
            blur_title_frame = ctk.CTkFrame(blur_frame, fg_color="transparent")
            blur_title_frame.pack(fill="x")
            
            lbl_blur_title = ctk.CTkLabel(
                blur_title_frame, 
                text="毛玻璃模糊半径 (Blur Radius):", 
                font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
                anchor="w"
            )
            lbl_blur_title.pack(side="left")
            
            lbl_blur_val = ctk.CTkLabel(
                blur_title_frame, 
                text="30", 
                font=ctk.CTkFont(size=13, weight="bold"),
                text_color="#00D2FF"
            )
            lbl_blur_val.pack(side="right")
            setattr(self, f"lbl_blur_val_{mod_key}", lbl_blur_val)
            
            blur_slider = ctk.CTkSlider(
                blur_frame,
                from_=0,
                to=60,
                number_of_steps=60,
                progress_color="#00D2FF",
                button_color="#00B2DF",
                button_hover_color="#0092BF",
                command=lambda val, k=mod_key: self.on_blur_slider(k, val)
            )
            blur_slider.pack(fill="x", pady=6)
            setattr(self, f"blur_slider_{mod_key}", blur_slider)
        
        # 3. Opacity Slider
        opacity_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
        opacity_frame.pack(fill="x", pady=10, padx=15)
        
        op_title_frame = ctk.CTkFrame(opacity_frame, fg_color="transparent")
        op_title_frame.pack(fill="x")
        
        lbl_op_title = ctk.CTkLabel(
            op_title_frame, 
            text="背景颜色浓度 (Tint Opacity):", 
            font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
            anchor="w"
        )
        lbl_op_title.pack(side="left")
        
        lbl_op_val = ctk.CTkLabel(
            op_title_frame, 
            text="50%", 
            font=ctk.CTkFont(size=13, weight="bold"),
            text_color="#00D2FF"
        )
        lbl_op_val.pack(side="right")
        setattr(self, f"lbl_op_val_{mod_key}", lbl_op_val)
        
        op_slider = ctk.CTkSlider(
            opacity_frame,
            from_=0,
            to=100,
            number_of_steps=100,
            progress_color="#00D2FF",
            button_color="#00B2DF",
            button_hover_color="#0092BF",
            command=lambda val, k=mod_key: self.on_opacity_slider(k, val)
        )
        op_slider.pack(fill="x", pady=6)
        setattr(self, f"op_slider_{mod_key}", op_slider)
        
        # 4. Luminosity Slider
        if mod_key != "FileExplorer":
            lum_frame = ctk.CTkFrame(parent_tab, fg_color="transparent")
            lum_frame.pack(fill="x", pady=10, padx=15)
            
            lum_title_frame = ctk.CTkFrame(lum_frame, fg_color="transparent")
            lum_title_frame.pack(fill="x")
            
            lbl_lum_title = ctk.CTkLabel(
                lum_title_frame, 
                text="色彩亮度/饱和度填充 (Tint Luminosity):", 
                font=ctk.CTkFont(family="Microsoft YaHei", size=13, weight="bold"),
                anchor="w"
            )
            lbl_lum_title.pack(side="left")
            
            lbl_lum_val = ctk.CTkLabel(
                lum_title_frame, 
                text="100%", 
                font=ctk.CTkFont(size=13, weight="bold"),
                text_color="#00D2FF"
            )
            lbl_lum_val.pack(side="right")
            setattr(self, f"lbl_lum_val_{mod_key}", lbl_lum_val)
            
            lum_slider = ctk.CTkSlider(
                lum_frame,
                from_=0,
                to=150,
                number_of_steps=150,
                progress_color="#00D2FF",
                button_color="#00B2DF",
                button_hover_color="#0092BF",
                command=lambda val, k=mod_key: self.on_luminosity_slider(k, val)
            )
            lum_slider.pack(fill="x", pady=6)
            setattr(self, f"lum_slider_{mod_key}", lum_slider)

    # Registry Read/Write Utilities
    def get_reg_value(self, path, name, default=None):
        try:
            key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, path, 0, winreg.KEY_READ)
            val, type = winreg.QueryValueEx(key, name)
            winreg.CloseKey(key)
            return val
        except FileNotFoundError:
            return default
        except Exception as e:
            print(f"Error reading registry {name}: {e}")
            return default

    def set_reg_value(self, path, name, value, is_dword=False):
        try:
            key = winreg.CreateKeyEx(winreg.HKEY_LOCAL_MACHINE, path, 0, winreg.KEY_SET_VALUE)
            if is_dword:
                winreg.SetValueEx(key, name, 0, winreg.REG_DWORD, int(value))
            else:
                winreg.SetValueEx(key, name, 0, winreg.REG_SZ, str(value))
            winreg.CloseKey(key)
        except Exception as e:
            print(f"Error writing registry {name}: {e}")

    # Settings Sync & Initial Load
    def load_all_settings(self):
        for mod_key, path in self.mods.items():
            keys = self.setting_keys.get(mod_key, {})
            defaults = self.setting_defaults.get(mod_key, {})
            if not keys:
                continue

            # 1. Color Preset
            if "color" in keys:
                color_preset = self.get_reg_value(path, keys["color"], defaults.get("color", "Default"))
                cn_preset = self.color_options_en_to_cn.get(color_preset, "主题默认")
                getattr(self, f"dropdown_{mod_key}").set(cn_preset)
            
            # 2. Blur Level
            if "blur" in keys and hasattr(self, f"blur_slider_{mod_key}"):
                blur_preset = self.get_reg_value(path, keys["blur"], defaults.get("blur", 30))
                getattr(self, f"blur_slider_{mod_key}").set(blur_preset)
                getattr(self, f"lbl_blur_val_{mod_key}").configure(text=str(int(blur_preset)))
            
            # 3. Opacity Level
            if "opacity" in keys:
                op_preset = self.get_reg_value(path, keys["opacity"], defaults.get("opacity", 50))
                getattr(self, f"op_slider_{mod_key}").set(op_preset)
                getattr(self, f"lbl_op_val_{mod_key}").configure(text=f"{int(op_preset)}%")

            # 4. Luminosity Level
            if "luminosity" in keys and hasattr(self, f"lum_slider_{mod_key}"):
                lum_preset = self.get_reg_value(path, keys["luminosity"], defaults.get("luminosity", 100))
                getattr(self, f"lum_slider_{mod_key}").set(lum_preset)
                getattr(self, f"lbl_lum_val_{mod_key}").configure(text=f"{int(lum_preset)}%")

        self.load_stage_manager_settings()

    def load_stage_manager_settings(self):
        path = self.mods["StageManager"]

        edge = self.get_reg_value(path, "sidebarEdge", 0)
        edge_labels = ["左侧 (Left)", "右侧 (Right)"]
        self.dropdown_StageManager_edge.set(edge_labels[edge if 0 <= edge <= 1 else 0])

        width = self.get_reg_value(path, "sidebarWidth", 80)
        self.slider_StageManager_width.set(width)
        self.lbl_sm_width_val.configure(text=f"{int(width)} px")

        opacity = self.get_reg_value(path, "acrylicOpacity", 180)
        self.slider_StageManager_opacity.set(opacity)
        self.lbl_sm_opacity_val.configure(text=f"{int(opacity)} / 255")

        max_stages = self.get_reg_value(path, "maxVisibleStages", 5)
        self.slider_StageManager_maxstages.set(max_stages)
        self.lbl_sm_maxstages_val.configure(text=str(int(max_stages)))

        hotkey = self.get_reg_value(path, "enableHotkey", 1)
        if hotkey:
            self.switch_StageManager_hotkey.select()
        else:
            self.switch_StageManager_hotkey.deselect()

        anim = self.get_reg_value(path, "animationDisabled", 1)
        if anim:
            self.switch_StageManager_anim.select()
        else:
            self.switch_StageManager_anim.deselect()

    def on_sm_edge_changed(self, value):
        edge_map = {"左侧 (Left)": 0, "右侧 (Right)": 1}
        edge_val = edge_map.get(value, 0)
        self.set_reg_value(self.mods["StageManager"], "sidebarEdge", edge_val, is_dword=True)
        print(f"Stage Manager sidebar edge changed to {edge_val}")

    def on_sm_width_slider(self, value):
        width_val = int(value)
        self.lbl_sm_width_val.configure(text=f"{width_val} px")
        self.set_reg_value(self.mods["StageManager"], "sidebarWidth", width_val, is_dword=True)
        print(f"Stage Manager sidebar width changed to {width_val}")

    def on_sm_opacity_slider(self, value):
        opacity_val = int(value)
        self.lbl_sm_opacity_val.configure(text=f"{opacity_val} / 255")
        self.set_reg_value(self.mods["StageManager"], "acrylicOpacity", opacity_val, is_dword=True)
        print(f"Stage Manager acrylic opacity changed to {opacity_val}")

    def on_sm_maxstages_slider(self, value):
        max_val = int(value)
        self.lbl_sm_maxstages_val.configure(text=str(max_val))
        self.set_reg_value(self.mods["StageManager"], "maxVisibleStages", max_val, is_dword=True)
        print(f"Stage Manager max visible stages changed to {max_val}")

    def on_sm_hotkey_toggle(self):
        is_on = self.switch_StageManager_hotkey.get()
        self.set_reg_value(self.mods["StageManager"], "enableHotkey", 1 if is_on else 0, is_dword=True)
        print(f"Stage Manager hotkey {'enabled' if is_on else 'disabled'}")

    def on_sm_anim_toggle(self):
        is_on = self.switch_StageManager_anim.get()
        self.set_reg_value(self.mods["StageManager"], "animationDisabled", 1 if is_on else 0, is_dword=True)
        print(f"Stage Manager animation {'disabled' if is_on else 'enabled'}")

    # Live Event Handlers
    def ensure_file_explorer_diy_theme(self, mod_key):
        return

    def on_register_explorer_blur_mica(self):
        import os
        try:
            import ctypes
            script_path = os.path.abspath(os.path.join("ExplorerBlurMica", "register.cmd"))
            if os.path.exists(script_path):
                ctypes.windll.shell32.ShellExecuteW(None, "runas", "cmd.exe", f"/c \"{script_path}\"", None, 1)
                print("Requesting elevated register.cmd execution...")
            else:
                print("register.cmd not found at:", script_path)
        except Exception as e:
            print(f"Failed to register ExplorerBlurMica: {e}")

    def on_uninstall_explorer_blur_mica(self):
        import os
        try:
            import ctypes
            script_path = os.path.abspath(os.path.join("ExplorerBlurMica", "uninstall.cmd"))
            if os.path.exists(script_path):
                ctypes.windll.shell32.ShellExecuteW(None, "runas", "cmd.exe", f"/c \"{script_path}\"", None, 1)
                print("Requesting elevated uninstall.cmd execution...")
            else:
                print("uninstall.cmd not found at:", script_path)
        except Exception as e:
            print(f"Failed to uninstall ExplorerBlurMica: {e}")

    def on_open_explorer_blur_mica_config(self):
        import os
        import subprocess
        try:
            editor_path = os.path.abspath(os.path.join("ExplorerBlurMica", "config-editor-wpf.exe"))
            if os.path.exists(editor_path):
                subprocess.Popen(editor_path, cwd=os.path.abspath("ExplorerBlurMica"))
                print("Launched ExplorerBlurMica configuration editor.")
            else:
                print("Configuration editor not found at:", editor_path)
        except Exception as e:
            print(f"Failed to open config editor: {e}")

    def on_color_preset_changed(self, mod_key, value_cn):
        value_en = self.color_options_cn_to_en[value_cn]
        path = self.mods[mod_key]
        keys = self.setting_keys[mod_key]
        self.ensure_file_explorer_diy_theme(mod_key)
        
        # Write color mode directly
        self.set_reg_value(path, keys["color"], value_en)
        print(f"Color preset for {mod_key} changed to {value_en}")

    def on_blur_slider(self, mod_key, value):
        blur_val = int(value)
        path = self.mods[mod_key]
        keys = self.setting_keys[mod_key]
        self.ensure_file_explorer_diy_theme(mod_key)
        
        # Update label and write directly to the mod-specific blur setting
        getattr(self, f"lbl_blur_val_{mod_key}").configure(text=str(blur_val))
        self.set_reg_value(path, keys["blur"], blur_val, is_dword=True)
        print(f"Blur Preset for {mod_key} updated to {blur_val}")

    def on_opacity_slider(self, mod_key, value):
        op_pct = int(value)
        path = self.mods[mod_key]
        keys = self.setting_keys[mod_key]
        self.ensure_file_explorer_diy_theme(mod_key)
        
        # Update label and write directly to the mod-specific opacity setting
        getattr(self, f"lbl_op_val_{mod_key}").configure(text=f"{op_pct}%")
        self.set_reg_value(path, keys["opacity"], op_pct, is_dword=True)
        print(f"Opacity Preset for {mod_key} updated to {op_pct}")

    def on_luminosity_slider(self, mod_key, value):
        lum_pct = int(value)
        path = self.mods[mod_key]
        keys = self.setting_keys[mod_key]
        self.ensure_file_explorer_diy_theme(mod_key)
        
        # Update label and write directly to the mod-specific luminosity setting
        getattr(self, f"lbl_lum_val_{mod_key}").configure(text=f"{lum_pct}%")
        self.set_reg_value(path, keys["luminosity"], lum_pct, is_dword=True)
        print(f"Luminosity Preset for {mod_key} updated to {lum_pct}")

    def sync_all_mods(self):
        # Synchronize Taskbar settings to all glass-capable modules.
        tb_path = self.mods["Taskbar"]
        
        color_preset = self.get_reg_value(tb_path, "bgColorMode", "Default")
        blur_preset = self.get_reg_value(tb_path, "blurPreset", 30)
        op_preset = self.get_reg_value(tb_path, "opacityPreset", 50)
        lum_preset = self.get_reg_value(tb_path, "luminosityPreset", 100)
        
        for name in ["StartMenu", "NotificationCenter"]:
            path = self.mods[name]
            keys = self.setting_keys[name]
            self.set_reg_value(path, keys["color"], color_preset)
            self.set_reg_value(path, keys["blur"], blur_preset, is_dword=True)
            self.set_reg_value(path, keys["opacity"], op_preset, is_dword=True)
            self.set_reg_value(path, keys["luminosity"], lum_preset, is_dword=True)
            
        # Reload in UI
        self.load_all_settings()
        print("Successfully synchronized all mods!")

    def restart_shell(self):
        import subprocess
        try:
            subprocess.Popen("restart_explorer.bat", shell=True)
            print("Restarting shell via restart_explorer.bat...")
        except Exception as e:
            print(f"Failed to restart shell: {e}")

if __name__ == "__main__":
    app = ZenCustomizerApp()
    app.mainloop()

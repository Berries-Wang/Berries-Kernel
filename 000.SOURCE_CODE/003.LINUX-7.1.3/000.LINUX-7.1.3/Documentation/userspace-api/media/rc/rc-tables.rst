.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later

.. _Remote_controllers_tables:

**************************************
Remote controller tables(遥控器键码表)
**************************************

Unfortunately, for several years, there was no effort to create uniform
IR keycodes for different devices. This caused the same IR keyname to be
mapped completely differently on different IR devices. This resulted
that the same IR keyname to be mapped completely different on different
IR's. Due to that, V4L2 API now specifies a standard for mapping Media
keys on IR.
(遗憾的是，多年来一直没有人努力为不同的设备创建统一的 IR 键码。这导致同一个
IR 键名在不同的 IR 设备上映射得完全不同，其结果是同一个 IR 键名在不同 IR 上
得到完全不同的映射。正因如此，V4L2 API 现在为 Media 键在 IR 上的映射规定了一个
标准。)

This standard should be used by both V4L/DVB drivers and userspace
applications
(V4L/DVB 驱动与用户态应用都应当遵循这一标准。)

The modules register the remote as keyboard within the linux input
layer. This means that the IR key strokes will look like normal keyboard
key strokes (if CONFIG_INPUT_KEYBOARD is enabled). Using the event
devices (CONFIG_INPUT_EVDEV) it is possible for applications to access
the remote via /dev/input/event devices.
(这些模块在 Linux 输入层内把遥控器注册为键盘。这意味着 IR 按键动作看起来就像
普通的键盘按键（如果启用了 CONFIG_INPUT_KEYBOARD）。借助 event 设备
（CONFIG_INPUT_EVDEV），应用可以通过 /dev/input/event 设备访问遥控器。)


.. _rc_standard_keymap:

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: IR default keymapping
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2


    -  .. row 1

       -  Key code (键码)

       -  Meaning (含义)

       -  Key examples on IR (IR 上的按键示例)

    -  .. row 2

       -  **Numeric keys** (数字键)

    -  .. row 3

       -  ``KEY_NUMERIC_0``

       -  Keyboard digit 0 (键盘数字 0)

       -  0

    -  .. row 4

       -  ``KEY_NUMERIC_1``

       -  Keyboard digit 1 (键盘数字 1)

       -  1

    -  .. row 5

       -  ``KEY_NUMERIC_2``

       -  Keyboard digit 2 (键盘数字 2)

       -  2

    -  .. row 6

       -  ``KEY_NUMERIC_3``

       -  Keyboard digit 3 (键盘数字 3)

       -  3

    -  .. row 7

       -  ``KEY_NUMERIC_4``

       -  Keyboard digit 4 (键盘数字 4)

       -  4

    -  .. row 8

       -  ``KEY_NUMERIC_5``

       -  Keyboard digit 5 (键盘数字 5)

       -  5

    -  .. row 9

       -  ``KEY_NUMERIC_6``

       -  Keyboard digit 6 (键盘数字 6)

       -  6

    -  .. row 10

       -  ``KEY_NUMERIC_7``

       -  Keyboard digit 7 (键盘数字 7)

       -  7

    -  .. row 11

       -  ``KEY_NUMERIC_8``

       -  Keyboard digit 8 (键盘数字 8)

       -  8

    -  .. row 12

       -  ``KEY_NUMERIC_9``

       -  Keyboard digit 9 (键盘数字 9)

       -  9

    -  .. row 13

       -  **Movie play control** (影片播放控制)

    -  .. row 14

       -  ``KEY_FORWARD``

       -  Instantly advance in time (在时间上即时快进)

       -  >> / FORWARD

    -  .. row 15

       -  ``KEY_BACK``

       -  Instantly go back in time (在时间上即时后退)

       -  <<< / BACK

    -  .. row 16

       -  ``KEY_FASTFORWARD``

       -  Play movie faster (以更快的速度播放影片)

       -  >>> / FORWARD

    -  .. row 17

       -  ``KEY_REWIND``

       -  Play movie back (倒退播放影片)

       -  REWIND / BACKWARD

    -  .. row 18

       -  ``KEY_NEXT``

       -  Select next chapter / sub-chapter / interval (选择下一个章节/子章节/区间)

       -  NEXT / SKIP

    -  .. row 19

       -  ``KEY_PREVIOUS``

       -  Select previous chapter / sub-chapter / interval (选择上一个章节/子章节/区间)

       -  << / PREV / PREVIOUS

    -  .. row 20

       -  ``KEY_AGAIN``

       -  Repeat the video or a video interval (重复播放视频或视频区间)

       -  REPEAT / LOOP / RECALL

    -  .. row 21

       -  ``KEY_PAUSE``

       -  Pause stream (暂停流)

       -  PAUSE / FREEZE

    -  .. row 22

       -  ``KEY_PLAY``

       -  Play movie at the normal timeshift (以正常时移速度播放影片)

       -  NORMAL TIMESHIFT / LIVE / >

    -  .. row 23

       -  ``KEY_PLAYPAUSE``

       -  Alternate between play and pause (在播放与暂停之间切换)

       -  PLAY / PAUSE

    -  .. row 24

       -  ``KEY_STOP``

       -  Stop stream (停止流)

       -  STOP

    -  .. row 25

       -  ``KEY_RECORD``

       -  Start/stop recording stream (开始/停止录制流)

       -  CAPTURE / REC / RECORD/PAUSE

    -  .. row 26

       -  ``KEY_CAMERA``

       -  Take a picture of the image (对画面拍照)

       -  CAMERA ICON / CAPTURE / SNAPSHOT

    -  .. row 27

       -  ``KEY_SHUFFLE``

       -  Enable shuffle mode (启用随机播放模式)

       -  SHUFFLE

    -  .. row 28

       -  ``KEY_TIME``

       -  Activate time shift mode (激活时移模式)

       -  TIME SHIFT

    -  .. row 29

       -  ``KEY_TITLE``

       -  Allow changing the chapter (允许更改章节)

       -  CHAPTER

    -  .. row 30

       -  ``KEY_SUBTITLE``

       -  Allow changing the subtitle (允许更改字幕)

       -  SUBTITLE

    -  .. row 31

       -  **Image control** (图像控制)

    -  .. row 32

       -  ``KEY_BRIGHTNESSDOWN``

       -  Decrease Brightness (降低亮度)

       -  BRIGHTNESS DECREASE

    -  .. row 33

       -  ``KEY_BRIGHTNESSUP``

       -  Increase Brightness (提高亮度)

       -  BRIGHTNESS INCREASE

    -  .. row 34

       -  ``KEY_ANGLE``

       -  Switch video camera angle (on videos with more than one angle
	  stored) (切换视频摄像机角度，适用于存储了多个角度的视频)

       -  ANGLE / SWAP

    -  .. row 35

       -  ``KEY_EPG``

       -  Open the Elecrowonic Play Guide (EPG) (打开电子节目指南)

       -  EPG / GUIDE

    -  .. row 36

       -  ``KEY_TEXT``

       -  Activate/change closed caption mode (激活/切换隐藏字幕模式)

       -  CLOSED CAPTION/TELETEXT / DVD TEXT / TELETEXT / TTX

    -  .. row 37

       -  **Audio control** (音频控制)

    -  .. row 38

       -  ``KEY_AUDIO``

       -  Change audio source (切换音源)

       -  AUDIO SOURCE / AUDIO / MUSIC

    -  .. row 39

       -  ``KEY_MUTE``

       -  Mute/unmute audio (静音/取消静音)

       -  MUTE / DEMUTE / UNMUTE

    -  .. row 40

       -  ``KEY_VOLUMEDOWN``

       -  Decrease volume (降低音量)

       -  VOLUME- / VOLUME DOWN

    -  .. row 41

       -  ``KEY_VOLUMEUP``

       -  Increase volume (提高音量)

       -  VOLUME+ / VOLUME UP

    -  .. row 42

       -  ``KEY_MODE``

       -  Change sound mode (切换声音模式)

       -  MONO/STEREO

    -  .. row 43

       -  ``KEY_LANGUAGE``

       -  Select Language (选择语言)

       -  1ST / 2ND LANGUAGE / DVD LANG / MTS/SAP / MTS SEL

    -  .. row 44

       -  **Channel control** (频道控制)

    -  .. row 45

       -  ``KEY_CHANNEL``

       -  Go to the next favorite channel (跳到下一个收藏频道)

       -  ALT / CHANNEL / CH SURFING / SURF / FAV

    -  .. row 46

       -  ``KEY_CHANNELDOWN``

       -  Decrease channel sequentially (按顺序减小频道号)

       -  CHANNEL - / CHANNEL DOWN / DOWN

    -  .. row 47

       -  ``KEY_CHANNELUP``

       -  Increase channel sequentially (按顺序增大频道号)

       -  CHANNEL + / CHANNEL UP / UP

    -  .. row 48

       -  ``KEY_DIGITS``

       -  Use more than one digit for channel (频道号使用多位数字)

       -  PLUS / 100/ 1xx / xxx / -/-- / Single Double Triple Digit

    -  .. row 49

       -  ``KEY_SEARCH``

       -  Start channel autoscan (开始频道自动搜索)

       -  SCAN / AUTOSCAN

    -  .. row 50

       -  **Colored keys** (彩色键)

    -  .. row 51

       -  ``KEY_BLUE``

       -  IR Blue key (IR 蓝色键)

       -  BLUE

    -  .. row 52

       -  ``KEY_GREEN``

       -  IR Green Key (IR 绿色键)

       -  GREEN

    -  .. row 53

       -  ``KEY_RED``

       -  IR Red key (IR 红色键)

       -  RED

    -  .. row 54

       -  ``KEY_YELLOW``

       -  IR Yellow key (IR 黄色键)

       -  YELLOW

    -  .. row 55

       -  **Media selection** (媒体选择)

    -  .. row 56

       -  ``KEY_CD``

       -  Change input source to Compact Disc (把输入源切换到 CD)

       -  CD

    -  .. row 57

       -  ``KEY_DVD``

       -  Change input to DVD (把输入切换到 DVD)

       -  DVD / DVD MENU

    -  .. row 58

       -  ``KEY_EJECTCLOSECD``

       -  Open/close the CD/DVD player (打开/关闭 CD/DVD 播放机)

       -  -> ) / CLOSE / OPEN

    -  .. row 59

       -  ``KEY_MEDIA``

       -  Turn on/off Media application (打开/关闭媒体应用)

       -  PC/TV / TURN ON/OFF APP

    -  .. row 60

       -  ``KEY_PC``

       -  Selects from TV to PC (从 TV 切换到 PC)

       -  PC

    -  .. row 61

       -  ``KEY_RADIO``

       -  Put into AM/FM radio mode (进入 AM/FM 收音机模式)

       -  RADIO / TV/FM / TV/RADIO / FM / FM/RADIO

    -  .. row 62

       -  ``KEY_TV``

       -  Select tv mode (选择电视模式)

       -  TV / LIVE TV

    -  .. row 63

       -  ``KEY_TV2``

       -  Select Cable mode (选择有线模式)

       -  AIR/CBL

    -  .. row 64

       -  ``KEY_VCR``

       -  Select VCR mode (选择录像机模式)

       -  VCR MODE / DTR

    -  .. row 65

       -  ``KEY_VIDEO``

       -  Alternate between input modes (在输入模式之间循环切换)

       -  SOURCE / SELECT / DISPLAY / SWITCH INPUTS / VIDEO

    -  .. row 66

       -  **Power control** (电源控制)

    -  .. row 67

       -  ``KEY_POWER``

       -  Turn on/off computer (打开/关闭计算机)

       -  SYSTEM POWER / COMPUTER POWER

    -  .. row 68

       -  ``KEY_POWER2``

       -  Turn on/off application (打开/关闭应用)

       -  TV ON/OFF / POWER

    -  .. row 69

       -  ``KEY_SLEEP``

       -  Activate sleep timer (激活睡眠定时器)

       -  SLEEP / SLEEP TIMER

    -  .. row 70

       -  ``KEY_SUSPEND``

       -  Put computer into suspend mode (让计算机进入挂起模式)

       -  STANDBY / SUSPEND

    -  .. row 71

       -  **Window control** (窗口控制)

    -  .. row 72

       -  ``KEY_CLEAR``

       -  Stop stream and return to default input video/audio (停止流并返回默认输入视频/音频)

       -  CLEAR / RESET / BOSS KEY

    -  .. row 73

       -  ``KEY_CYCLEWINDOWS``

       -  Minimize windows and move to the next one (最小化窗口并切换到下一个)

       -  ALT-TAB / MINIMIZE / DESKTOP

    -  .. row 74

       -  ``KEY_FAVORITES``

       -  Open the favorites stream window (打开收藏流窗口)

       -  TV WALL / Favorites

    -  .. row 75

       -  ``KEY_MENU``

       -  Call application menu (调出应用菜单)

       -  2ND CONTROLS (USA: MENU) / DVD/MENU / SHOW/HIDE CTRL

    -  .. row 76

       -  ``KEY_NEW``

       -  Open/Close Picture in Picture (打开/关闭画中画)

       -  PIP

    -  .. row 77

       -  ``KEY_OK``

       -  Send a confirmation code to application (向应用发送确认码)

       -  OK / ENTER / RETURN

    -  .. row 78

       -  ``KEY_ASPECT_RATIO``

       -  Select screen aspect ratio (选择屏幕宽高比)

       -  4:3 16:9 SELECT

    -  .. row 79

       -  ``KEY_FULL_SCREEN``

       -  Put device into zoom/full screen mode (让设备进入缩放/全屏模式)

       -  ZOOM / FULL SCREEN / ZOOM+ / HIDE PANEL / SWITCH

    -  .. row 80

       -  **Navigation keys** (导航键)

    -  .. row 81

       -  ``KEY_ESC``

       -  Cancel current operation (取消当前操作)

       -  CANCEL / BACK

    -  .. row 82

       -  ``KEY_HELP``

       -  Open a Help window (打开帮助窗口)

       -  HELP

    -  .. row 83

       -  ``KEY_HOMEPAGE``

       -  Navigate to Homepage (转到主页)

       -  HOME

    -  .. row 84

       -  ``KEY_INFO``

       -  Open On Screen Display (打开屏幕信息显示)

       -  DISPLAY INFORMATION / OSD

    -  .. row 85

       -  ``KEY_WWW``

       -  Open the default browser (打开默认浏览器)

       -  WEB

    -  .. row 86

       -  ``KEY_UP``

       -  Up key (向上键)

       -  UP

    -  .. row 87

       -  ``KEY_DOWN``

       -  Down key (向下键)

       -  DOWN

    -  .. row 88

       -  ``KEY_LEFT``

       -  Left key (向左键)

       -  LEFT

    -  .. row 89

       -  ``KEY_RIGHT``

       -  Right key (向右键)

       -  RIGHT

    -  .. row 90

       -  **Miscellaneous keys** (其他键)

    -  .. row 91

       -  ``KEY_DOT``

       -  Return a dot (输入一个点)

       -  .

    -  .. row 92

       -  ``KEY_FN``

       -  Select a function (选择一个功能)

       -  FUNCTION


It should be noted that, sometimes, there some fundamental missing keys
at some cheaper IR's. Due to that, it is recommended to:
(应当注意的是，一些廉价的遥控器有时会缺少某些基本按键。正因如此，建议：)


.. _rc_keymap_notes:

.. flat-table:: Notes
    :header-rows:  0
    :stub-columns: 0


    -  .. row 1

       -  On simpler IR's, without separate channel keys, you need to map UP
	  as ``KEY_CHANNELUP`` (在没有独立频道键的较简单遥控器上，需要把 UP 映射为 KEY_CHANNELUP)

    -  .. row 2

       -  On simpler IR's, without separate channel keys, you need to map
	  DOWN as ``KEY_CHANNELDOWN`` (在没有独立频道键的较简单遥控器上，需要把 DOWN 映射为 KEY_CHANNELDOWN)

    -  .. row 3

       -  On simpler IR's, without separate volume keys, you need to map
	  LEFT as ``KEY_VOLUMEDOWN`` (在没有独立音量键的较简单遥控器上，需要把 LEFT 映射为 KEY_VOLUMEDOWN)

    -  .. row 4

       -  On simpler IR's, without separate volume keys, you need to map
	  RIGHT as ``KEY_VOLUMEUP`` (在没有独立音量键的较简单遥控器上，需要把 RIGHT 映射为 KEY_VOLUMEUP)

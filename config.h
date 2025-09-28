#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 0;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "BigBlueTermPlus Nerd Font:size=8" };
static const char dmenufont[]       = "BigBlueTermPlus Nerd Font:size=8";
static const char col_gray1[]       = "#c0c0c0";
static const char col_gray2[]       = "#d3d7cf";
static const char col_gray3[]       = "#000000";
static const char col_gray4[]       = "#c0c0c0";
static const char col_cyan[]        = "#05056d";
static const char *colors[][3]      = {
    /*               fg         bg         border   */
    [SchemeNorm] = { col_gray3, col_gray1, col_cyan },
    [SchemeSel]  = { col_gray4, col_cyan,  col_gray2  },
};

typedef struct {
    const char *name;
    const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spfm", "-g", "144x41", "-e", "lf", NULL };
const char *spcmd3[] = {"keepassxc", NULL };
const char *spcmd4[] = {"alacritty", "--class", "spalacritty", "-o", "window.dimensions.columns=96", "window.dimensions.lines=27", NULL};

static Sp scratchpads[] = {
    /* name          cmd  */
    {"spterm",      spcmd1},
    {"splf",        spcmd2},
    {"keepassxc",   spcmd3},
    {"spalacritty", spcmd4},
};

/* tagging */
static const char *tags[] = { "WWW", ">_", "III", "IV", "V", "VI", "VII", "-_- ", "SUCKS" };

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
	/* class          instance        title         tags mask  isfloating  isterminal  noswallow  monitor */
    { "Gimp",           NULL,         NULL,            0,         1,          0,           0,        -1 },
    { "Firefox",        NULL,         NULL,            1 << 8,    0,          0,          -1,        -1 },
    { "st-256color",    NULL,         NULL,            0,         0,          1,           0,        -1 },
    { "Alacritty",      NULL,         NULL,            0,         0,          1,           0,        -1 },
    { NULL,             NULL,         "Event Tester",  0,         0,          0,           1,        -1 }, /* xev */
    /* Scratchpads */
    { NULL,            "spterm",      NULL,        SPTAG(0),	  1,          1,           0,        -1 },
    { NULL,            "spfm",        NULL,        SPTAG(1),	  1,          1,           0,        -1 },
    { NULL,            "keepassxc",   NULL,        SPTAG(2),	  1,          0,           1,        -1 },
    { NULL,            "spalacritty", NULL,        SPTAG(3),	  1,          1,           0,        -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[]=",      tile },    /* first entry is default */
    { "><>",      NULL },    /* no layout function means floating behavior */
    { "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
    { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

/* DMG Commands */
static const char *roficmd[]         = { "rofi", "-show", "drun", NULL };
static const char *alacrittycmd[]    = { "alacritty", NULL };
static const char *browser[]         = { "chromium", NULL };
static const char *filemanager[]     = { "pcmanfm", NULL };

static const char *mute[]            = { "amixer", "-q", "set", "Master", "toggle", NULL };
static const char *max_vol[]         = { "amixer", "-q", "set", "Master", "100%", NULL };
static const char *volume_up[]       = { "amixer", "-q", "set", "Master", "5%+", "unmute", NULL };
static const char *volume_down[]     = { "amixer", "-q", "set", "Master", "5%- ", "unmute", NULL };
static const char *brightness_up[]   = { "xbacklight", "-inc", "5", NULL };
static const char *brightness_down[] = { "xbacklight", "-dec", "5", NULL };

static const Key keys[] = {
    /* modifier                     key        function        argument */
    { MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
    { MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,                       XK_b,      togglebar,      {0} },
    { MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
    { MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
    { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
    { MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
    { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
    { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
    { MODKEY,                       XK_Return, zoom,           {0} },
    { MODKEY,                       XK_Tab,    view,           {0} },
    { MODKEY,                       XK_q,      killclient,     {0} },
    { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
    { MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
    { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
    { MODKEY,                       XK_space,  setlayout,      {0} },
    { MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
    { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
    { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
    { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },

    /* DMG Keys*/
    { MODKEY|ShiftMask,             XK_p,         spawn,       {.v = roficmd } },
    { MODKEY|ShiftMask,             XK_backslash, spawn,       {.v = alacrittycmd } },
    { MODKEY,                       XK_w,         spawn,       {.v = browser } },
    { MODKEY,                       XK_o,         spawn,       {.v = filemanager } },
    { 0,                            XK_Print,     spawn,       SHCMD("maim ~/Pictures/screenshots/screens/screenshot-$(date +%F-%T).png") },
    { MODKEY,                       XK_Print,     spawn,       SHCMD("maim -s ~/Pictures/screenshots/regions/screenshot-$(date +%F-%T).png") },
    { 0,                            XK_Scroll_Lock, spawn,     SHCMD("setxkbmap -query | grep -q 'us' && setxkbmap ar || setxkbmap us") },

    { 0,                     XF86XK_AudioMute, spawn,          {.v = mute } },
    { MODKEY,                XK_KP_Add,        spawn,          {.v = max_vol } },
    { 0,                     XF86XK_AudioLowerVolume,  spawn,  {.v = volume_down } },
    { 0,                     XF86XK_AudioRaiseVolume,  spawn,  {.v = volume_up } },
    { 0,                     XF86XK_MonBrightnessUp,   spawn,  {.v = brightness_up } },
    { 0,                     XF86XK_MonBrightnessDown, spawn,  {.v = brightness_down } },
    /* Scratchpads keys */
    { Mod1Mask,                 XK_o,           togglescratch,  {.ui = 1 } }, /* lf file manager*/
    { MODKEY,                   XK_x,           togglescratch,  {.ui = 2 } }, /* keepassxc*/
    { Mod1Mask,                 XK_backslash,   togglescratch,  {.ui = 3 } }, /* alacritty scratchpad */
    { Mod1Mask,                 XK_Return,      togglescratch,  {.ui = 0 } }, /* st terminal */


    TAGKEYS(                        XK_1,                      0)
    TAGKEYS(                        XK_2,                      1)
    TAGKEYS(                        XK_3,                      2)
    TAGKEYS(                        XK_4,                      3)
    TAGKEYS(                        XK_5,                      4)
    TAGKEYS(                        XK_6,                      5)
    TAGKEYS(                        XK_7,                      6)
    TAGKEYS(                        XK_8,                      7)
    TAGKEYS(                        XK_9,                      8)
    { MODKEY|ShiftMask,             XK_c,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkWinTitle,          0,              Button2,        zoom,           {0} },
    { ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};


I have windows 10 with installed visual studio for c++ development. And i have the java windowed app that i need to write in c++. 
so this is my java app:
//Buttons on top, image is behind

import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.text.SimpleDateFormat;
import java.time.LocalDate;
import java.time.temporal.ChronoUnit;
import java.util.Arrays;
import java.util.Date;
import java.util.Properties;
import org.jdatepicker.JDatePicker;
import org.jdatepicker.UtilDateModel;

public class MovingSquare extends JFrame {
    private static final String CONFIG_FILE = "config.txt";
    private JLabel imageLabel;
    private ImageIcon imageIcon = new ImageIcon("background.jpg");
    private ImageIcon appIcon = new ImageIcon("appicon.png");
    private JTextField periodField = new JTextField();
    private UtilDateModel dateModel = new UtilDateModel();
    private JDatePicker datePicker = new JDatePicker(dateModel);
    private int defaultPeriod = 31;
    private int periodLength;
    private String startDate;
    private LocalDate today = LocalDate.now();
    

    public MovingSquare() {
        loadFile();
        initializeUI();
    }

    private void initializeUI() {
        setTitle("Cycle visualizer");
        setIconImage(appIcon.getImage());
        setSize(1280, 760);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        imageLabel = new JLabel() {
            @Override
            public void paintComponent(Graphics g) {
                super.paintComponent(g);
                int height = getHeight();
                int width = getWidth();
                drawImageAndOverlay(g, width, height);
            }
        };
        
        JPanel inputPanel = new JPanel();
        JLabel periodDateLabel = new JLabel("Period start date:");
        JLabel periodFieldLabel = new JLabel("Period length:");
        JButton saveButton = new JButton("Save");

        saveButton.addActionListener(e -> saveFile());

        periodDateLabel.setPreferredSize(new Dimension(100, 25));
        datePicker.setPreferredSize(new Dimension(110, 25));
        periodFieldLabel.setPreferredSize(new Dimension(80, 25));
        periodField.setPreferredSize(new Dimension(40, 25));
        saveButton.setPreferredSize(new Dimension(70, 25));

        inputPanel.setLayout(new GridBagLayout());
        inputPanel.setOpaque(false);

        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(3, 5, 0, 5);

        inputPanel.add(periodDateLabel, gbc);
        inputPanel.add(datePicker, gbc);
        inputPanel.add(periodFieldLabel, gbc);
        inputPanel.add(periodField, gbc);
        inputPanel.add(saveButton, gbc);

        setContentPane(imageLabel);
        setLayout(new BorderLayout());
        add(inputPanel, BorderLayout.NORTH);

        setVisible(true);
        Timer timer = new Timer(100, e -> imageLabel.repaint());
        timer.start();
    }

    public void drawImageAndOverlay(Graphics g, int componentWidth, int componentHeight) {

        //Image setup
        double scaleH = (double) componentHeight / imageIcon.getIconHeight();
        double scaleW = (double) componentWidth / imageIcon.getIconWidth();
        double scale = Math.min(scaleH, scaleW);
        int imageWidth = (int) (imageIcon.getIconWidth() * scale);
        int imageHeight = (int) (imageIcon.getIconHeight() * scale);
        int offsetX = (componentWidth - imageWidth) / 2;
        int offsetY = (componentHeight - imageHeight) / 2;
        g.drawImage(imageIcon.getImage(), offsetX, offsetY, imageWidth, imageHeight, this); //Draw the image

        //Font setup
        int fontSize = (int) (imageHeight * 0.03);
        Font font = new Font("Times New Roman", Font.BOLD, fontSize);
        g.setFont(font); //Accept the font

        //Square setup
        int sideOffset = (int) (fontSize * 1);
        periodLength = extractValue(periodField.getText());
        int daysPassed = (int) ChronoUnit.DAYS.between(LocalDate.parse(selectDate()), today);
        int squareX = offsetX + sideOffset + (imageWidth - sideOffset * 2) * daysPassed / periodLength;
        g.setColor(Color.GREEN);
        g.fillRect(squareX, offsetY, 2, imageHeight - fontSize); //Draw the square

        //"Today" setup
        String stringVal = "Today";
        int todayWidth = g.getFontMetrics().stringWidth(stringVal);
        int todayX = squareX - todayWidth / 2;
        int todayY = offsetY + imageHeight - 5;
        g.drawString(stringVal, todayX, todayY); //Draw "Today" string

        //Days numbers setup
        int dayY = (int) (offsetY + imageHeight - imageHeight * 0.16);
        g.setColor(Color.BLUE);
        for (int i = 0; i <= periodLength; i++) {
            String dayNumber = String.valueOf(i);
            int dayNumberWidth = g.getFontMetrics().stringWidth(dayNumber);
            int dayX = offsetX + sideOffset + ((imageWidth - sideOffset * 2) * i / periodLength) - dayNumberWidth / 2;
            g.drawString(dayNumber, dayX, dayY);
        };        
    }

    public void loadFile() {
        try (BufferedReader reader = new BufferedReader(new FileReader(CONFIG_FILE))) {
            Properties props = new Properties();
            props.load(reader);
            startDate = props.getProperty("startDate", today.toString());
            periodLength = extractValue(props.getProperty("length", String.valueOf(defaultPeriod)));
            if (!startDate.matches("\\d{4}-\\d{2}-\\d{2}")) throw new IOException("Incorrect date format");
        } catch (IOException e) {
            e.printStackTrace();
            periodLength = defaultPeriod;
            startDate = today.toString();
        }
        int [] dateInts = Arrays.stream(startDate.split("-")).mapToInt(Integer::parseInt).toArray();
        periodField.setText(String.valueOf(periodLength));
        dateModel.setDate(dateInts[0], dateInts[1] - 1, dateInts[2]);
        dateModel.setSelected(true);
    }

    private void saveFile() {
        startDate = selectDate();
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(CONFIG_FILE))) {
            writer.write("startDate=" + startDate);
            writer.newLine();
            writer.write("length=" + periodLength);
            writer.newLine();
        } catch (IOException e) {
            e.printStackTrace();
        };
    }
    private String selectDate() {
        Date date = dateModel.getValue();
        if (date == null) return today.toString(); 
        return (new SimpleDateFormat("yyyy-MM-dd")).format(date);
    }

    public int extractValue(String value) {
        String extractedValue = value.replaceAll("\\D.*", "");
        try {
            return Integer.parseInt(extractedValue);
        } catch (NumberFormatException e) {
            return defaultPeriod; // Default value if parsing failed
        }
    }
    public static void main(String[] args) {
        SwingUtilities.invokeLater(MovingSquare::new);
    }
}

and this is the template from visual studio:

// Cycle visualizer.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Cycle visualizer.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CYCLEVISUALIZER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CYCLEVISUALIZER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));

    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CYCLEVISUALIZER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPICON));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

this is framework.h file:

// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
// -- included necesary libraries
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <algorithm>

this is Resource.h file:

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by Cycle visualizer.rc
//
#define IDC_MYICON                      2
#define IDD_CYCLEVISUALIZER_DIALOG      102
#define IDS_APP_TITLE                   103
#define IDD_ABOUTBOX                    103
#define IDM_ABOUT                       104
#define IDM_EXIT                        105
#define IDI_APPICON                     107
#define IDC_CYCLEVISUALIZER             109
#define IDR_MAINFRAME                   128
#define IDC_STATIC                      -1

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NO_MFC                     1
#define _APS_NEXT_RESOURCE_VALUE        129
#define _APS_NEXT_COMMAND_VALUE         32771
#define _APS_NEXT_CONTROL_VALUE         1000
#define _APS_NEXT_SYMED_VALUE           110
#endif
#endif

this is targetver.h file:

#pragma once

// // Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>

this is Cycle visualizer.h file:

#pragma once

#include "resource.h"

this is Cycle visualizer.rc file:
// Microsoft Visual C++ generated resource script.
//
#include "resource.h"

#define APSTUDIO_READONLY_SYMBOLS
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 2 resource.
//
#ifndef APSTUDIO_INVOKED
#include "targetver.h"
#endif
#define APSTUDIO_HIDDEN_SYMBOLS
#include "windows.h"
#undef APSTUDIO_HIDDEN_SYMBOLS

/////////////////////////////////////////////////////////////////////////////
#undef APSTUDIO_READONLY_SYMBOLS

/////////////////////////////////////////////////////////////////////////////
// English (United States) resources

#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)
LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US

/////////////////////////////////////////////////////////////////////////////
//
// Icon
//

// Icon with lowest ID value placed first to ensure application icon
// remains consistent on all systems.
IDI_APPICON             ICON                    "appicon.ico"


/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

IDC_CYCLEVISUALIZER MENU
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "E&xit",                       IDM_EXIT
    END
    POPUP "&Help"
    BEGIN
        MENUITEM "&About ...",                  IDM_ABOUT
    END
END


/////////////////////////////////////////////////////////////////////////////
//
// Accelerator
//

IDC_CYCLEVISUALIZER ACCELERATORS
BEGIN
    "?",            IDM_ABOUT,              ASCII,  ALT
    "/",            IDM_ABOUT,              ASCII,  ALT
END


/////////////////////////////////////////////////////////////////////////////
//
// Dialog
//

IDD_ABOUTBOX DIALOGEX 0, 0, 170, 62
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "About Cycle visualizer"
FONT 8, "MS Shell Dlg", 0, 0, 0x1
BEGIN
    ICON            IDR_MAINFRAME,IDC_STATIC,14,14,21,20
    LTEXT           "Cycle visualizer, Version 1.0",IDC_STATIC,42,14,114,8,SS_NOPREFIX
    LTEXT           "Copyright (c) 2023",IDC_STATIC,42,26,114,8
    DEFPUSHBUTTON   "OK",IDOK,60,41,50,14,WS_GROUP
END


/////////////////////////////////////////////////////////////////////////////
//
// DESIGNINFO
//

#ifdef APSTUDIO_INVOKED
GUIDELINES DESIGNINFO
BEGIN
    IDD_ABOUTBOX, DIALOG
    BEGIN
        LEFTMARGIN, 7
        RIGHTMARGIN, 163
        VERTGUIDE, 85
        TOPMARGIN, 7
        BOTTOMMARGIN, 55
    END
END
#endif    // APSTUDIO_INVOKED


#ifdef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE
//

1 TEXTINCLUDE 
BEGIN
    "resource.h\0"
END

2 TEXTINCLUDE 
BEGIN
    "#ifndef APSTUDIO_INVOKED\r\n"
    "#include ""targetver.h""\r\n"
    "#endif\r\n"
    "#define APSTUDIO_HIDDEN_SYMBOLS\r\n"
    "#include ""windows.h""\r\n"
    "#undef APSTUDIO_HIDDEN_SYMBOLS\r\n"
    "\0"
END

3 TEXTINCLUDE 
BEGIN
    "\r\n"
    "\0"
END

#endif    // APSTUDIO_INVOKED


/////////////////////////////////////////////////////////////////////////////
//
// String Table
//

STRINGTABLE
BEGIN
    IDS_APP_TITLE           "Cycle visualizer"
    IDC_CYCLEVISUALIZER     "CYCLEVISUALIZER"
END

#endif    // English (United States) resources
/////////////////////////////////////////////////////////////////////////////



#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 3 resource.
//


/////////////////////////////////////////////////////////////////////////////
#endif    // not APSTUDIO_INVOKED


I like this template and i would like to add the stuff from java app.




response






#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <fstream>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
WCHAR backgroundImagePath[MAX_PATH] = L"background.jpg";
WCHAR appIconPath[MAX_PATH] = L"appicon.png";
std::wstring configFile = L"config.txt";

int periodLength;
std::wstring startDate;
std::wstring today;

HWND hwnd;
HBITMAP hbmBackground;
HBITMAP hbmAppIcon;
HWND periodField;
HWND datePicker;
HWND saveButton;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void LoadConfigFile()
{
    std::wifstream file(configFile);
    if (file.is_open())
    {
        std::wstring line;
        std::getline(file, line);
        startDate = line.substr(line.find(L"=") + 1);

        std::getline(file, line);
        periodLength = std::stoi(line.substr(line.find(L"=") + 1));

        file.close();
    }
    else
    {
        periodLength = 31;
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm* localTime = std::localtime(&currentTime);
        std::wostringstream oss;
        oss << (localTime->tm_year + 1900) << L"-" << (localTime->tm_mon + 1) << L"-" << localTime->tm_mday;
        startDate = oss.str();
    }
}

void SaveConfigFile()
{
    std::wofstream file(configFile);
    if (file.is_open())
    {
        file << L"startDate=" << startDate << std::endl;
        file << L"length=" << periodLength << std::endl;
        file.close();
    }
}

void DrawImageAndOverlay(HDC hdc, int componentWidth, int componentHeight)
{
    // Draw background image
    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, hbmBackground);
    StretchBlt(hdc, 0, 0, componentWidth, componentHeight, hdcMem, 0, 0, 1280, 760, SRCCOPY);
    DeleteDC(hdcMem);

    // Draw square
    int fontSize = static_cast<int>(componentHeight * 0.03);
    int sideOffset = static_cast<int>(fontSize * 1);
    int daysPassed = static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count()) / 24 % periodLength;
    int squareX = sideOffset + (componentWidth - sideOffset * 2) * daysPassed / periodLength;
    RECT squareRect = { squareX, 0, squareX + 2, componentHeight - fontSize };
    FillRect(hdc, &squareRect, (HBRUSH)GetStockObject(GREEN_BRUSH));

    // Draw "Today" string
    std::wstring todayString = L"Today";
    RECT todayRect;
    DrawText(hdc, todayString.c_str(), todayString.length(), &todayRect, DT_CALCRECT);
    int todayX = squareX - (todayRect.right...
/////////////////////////////////////////////////////////////////////////////
//
// String Table
//

IDD_ABOUTBOX DIALOGEX 0, 0, 250, 150
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "About Cycle visualizer"
FONTTYPE SYSTEM
FONT "MS Shell Dlg", 8, 0, 0, 0x1
BEGIN
    ICON            IDR_MAINFRAME,IDC_MYICON,14,14,21,20
    LTEXT           "Cycle visualizer",IDC_STATIC,44,13,186,14
    LTEXT           "© 2022",IDC_STATIC,44,34,186,8
    LTEXT           "Made with C++ and WinAPI",IDC_STATIC,44,43,186,8
    DEFPUSHBUTTON   "OK",IDOK,85,57,75,14
END

IDD_CYCLEVISUALIZER_DIALOG DIALOGEX 0, 0, 0, 0
STYLE WS_POPUP | WS_VISIBLE
FONT 8, "MS Shell Dlg", 0, 0, 0x1
BEGIN
    LTEXT           "Cycle visualizer",IDC_STATIC,7,3,80,8
    CONTROL         "",IDC_STATIC,"STATIC",SS_GRAYFRAME,5,13,270,198
    DEFPUSHBUTTON   "About",IDM_ABOUT,230,218,43,14
    PUSHBUTTON      "Exit",IDM_EXIT,230,235,43,14
END

/////////////////////////////////////////////////////////////////////////////
#endif    // English (United States) resources
/////////////////////////////////////////////////////////////////////////////

#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 3 resource.
//

/////////////////////////////////////////////////////////////////////////////
#endif    // not APSTUDIO_INVOKED

I would like to ask how to implement Java code to c++.




response





#include "framework.h"
#include "Cycle visualizer.h"
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <algorithm>

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szImagePath[MAX_PATH];                    // path to the background image
WCHAR szIconPath[MAX_PATH];                      // path to the application icon
HWND hMainWnd;                                  // main window handle
HWND hPeriodField;                              // handle to the period field control
HWND hDatePicker;                               // handle to the date picker control
int periodLength;                               // period length
SYSTEMTIME startDate;                           // start date
HBITMAP hBackgroundImage;                       // handle to the background image

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void LoadBackgroundImage();
void DrawImageAndOverlay(HDC hdc, int componentWidth, int componentHeight);
void SaveConfiguration();
void LoadConfiguration();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CYCLEVISUALIZER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CYCLEVISUALIZER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));

    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CYCLEVISUALIZER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance,

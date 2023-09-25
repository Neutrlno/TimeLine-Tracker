//- Common Code For All Addons needed just to ease inclusion as separate files in user code ----------------------
#include <imgui.h>
#include <imgui_internal.h>
//-----------------------------------------------------------------------------------------------------------------

#include "imguidatechooser.h"
#include <time.h>               // very simple and common plain C header file (it's NOT the c++ <sys/time.h>). If not available it's probably better to implement it yourself rather than modifying this file.
#include <ctype.h>  // toupper()

namespace ImGui {

    inline static tm GetCurrentDate() {
        time_t now;
        time(&now);
        struct tm tmDest;
        localtime_s(&tmDest, &now);
        return tmDest;
    }

    // Tip: we modify tm (its fields can even be negative!) and then we call this method to retrieve a valid date
    inline static void RecalculateDateDependentFields(tm& date) {
        date.tm_isdst = -1;   // This tries to detect day time savings too
        time_t tmp = mktime(&date);
        struct tm tmDest;
        localtime_s(&tmDest, &tmp);
        date = tmDest;
    }

    inline static tm GetDateZero() {
        tm date;
        memset(&date, 0, sizeof(tm));     // Mandatory for emscripten. Please do not remove!
        //date.tm_isdst=-1;
        //date.tm_sec=0;		//	 Seconds.	[0-60] (1 leap second)
        //date.tm_min=0;		//	 Minutes.	[0-59]
        //date.tm_hour=0;		//	 Hours.	[0-23]
        //date.tm_mday=0;		//	 Day.		[1-31]
        //date.tm_mon=0;		//	 Month.	[0-11]
        //date.tm_year=0;		//	 Year	- 1900.
        //date.tm_wday=0;     //	 Day of week.	[0-6]
        //date.tm_yday=0;		//	 Days in year.[0-365]
        return date;
    }
    void SetDateZero(tm* date) {
        if (!date) return;
        *date = GetDateZero();
        return;
    }
    void SetDateToday(tm* date) {
        if (!date) return;
        *date = GetCurrentDate();
        return;
    }

    // Not as good as the original version...
    bool DateChooser(const char* label,
        tm& dateOut,
        const char* dateFormat,
        bool closeWhenMouseLeavesIt,
        bool* pSetStartDateToDateOutThisFrame,
        const char* leftArrow,
        const char* rightArrow,
        const char* upArrowString,
        const char* downArrowString)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;
        const ImGuiID id = window->GetID(label);

        static tm d = GetDateZero();
        static ImGuiID lastOpenComboID = 0;
        if (pSetStartDateToDateOutThisFrame && *pSetStartDateToDateOutThisFrame) {
            *pSetStartDateToDateOutThisFrame = false;
            if (dateOut.tm_mday == 0) dateOut = GetCurrentDate();
            d = dateOut;
            // move d at the first day of the month: mandatory fo the algo I'll use below
            d.tm_mday = 1;
            RecalculateDateDependentFields(d);  // now d.tm_wday is correct
        }
        else if (dateOut.tm_mday == 0) {
            dateOut = GetCurrentDate();
            d = dateOut;
            // move d at the first day of the month: mandatory fo the algo I'll use below
            d.tm_mday = 1;
            RecalculateDateDependentFields(d);  // now d.tm_wday is correct
        }
        else if (d.tm_mday == 0) {
            d = GetCurrentDate();
            // move d at the first day of the month: mandatory fo the algo I'll use below
            d.tm_mday = 1;
            RecalculateDateDependentFields(d);  // now d.tm_wday is correct
        }
        else if (d.tm_mday != 1) {
            // move d at the first day of the month: mandatory fo the algo I'll use below
            d.tm_mday = 1;
            RecalculateDateDependentFields(d);  // now d.tm_wday is correct
        }

        static const int nameBufferSize = 64;
        static char dayNames[7][nameBufferSize] = { "","","","","","","" };
        static char monthNames[12][nameBufferSize] = { "","","","","","","","","","","","" };
        static int maxMonthWidthIndex = -1;
        static int maxDayWidthIndex = -1;
        if (strlen(dayNames[0]) == 0) {
            tm tmp = GetDateZero();
            float maxDayWidth = 0;
            for (int i = 0; i < 7; i++) {
                tmp.tm_wday = i == 6 ? 0 : i + 1; //Makes the Monday as the first element of the dayNames array, tmp.tm_wday = i - makes Sunday as first day
                char* dayName = &dayNames[i][0];
                strftime(dayName, nameBufferSize, "%a", &tmp);
                if (strlen(dayName) == 0) {
                    static const char* weekdayFallbacks[7] = { "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su" };
                    strncpy_s(dayName, nameBufferSize, weekdayFallbacks[i], _TRUNCATE);
                }
                else {
                    dayName[0] = toupper(dayName[0]);
                    dayName[2] = '\0';
                };
                float dw = ImGui::CalcTextSize(dayName).x;
                if (maxDayWidth < dw) {
                    maxDayWidth = dw;
                    maxDayWidthIndex = i;
                }
            }
            float maxMonthWidth = 0;
            for (int i = 0; i < 12; i++) {
                tmp.tm_mon = i;
                char* monthName = &monthNames[i][0];
                strftime(monthName, nameBufferSize, "%B", &tmp);
                if (strlen(monthName) == 0) {
                    static const char* fallbacks[12] = { "January","February","March","April","May","June","July","August","September","October","November","December" };
                    strcpy_s(monthName, sizeof(monthName), fallbacks[i]);
                }
                else monthName[0] = toupper(monthName[0]);
                float mw = ImGui::CalcTextSize(monthName).x;
                if (maxMonthWidth < mw) {
                    maxMonthWidth = mw;
                    maxMonthWidthIndex = i;
                }
            }
        }

        ImGuiStyle& style = g.Style;

        const char* arrowLeft = leftArrow ? leftArrow : "<";
        const char* arrowRight = rightArrow ? rightArrow : ">";
        static char currentDateText[64];
        strftime(currentDateText, 64, dateFormat, &dateOut);

        const float arrowLeftWidth = ImGui::CalcTextSize(arrowLeft).x;
        const float arrowRightWidth = ImGui::CalcTextSize(arrowRight).x;
        ImVec2 textSize = ImGui::CalcTextSize("9");
        ImVec2 buttonSize(textSize.x * 2.0f + style.FramePadding.x * 2.0f, textSize.y);
        ImVec2 dayNameSize(ImGui::CalcTextSize(dayNames[maxDayWidthIndex]));

        const float widthByComboText = ImGui::CalcTextSize(currentDateText).x + style.FramePadding.x * 2.0f + textSize.x * 3.0f;
        const float widthByTopLine = (style.WindowPadding.x + arrowLeftWidth + arrowRightWidth + style.FramePadding.x * 4.0f) * 2.0f + style.ItemSpacing.x * 5.0f + ImGui::CalcTextSize(monthNames[maxMonthWidthIndex]).x + ImGui::CalcTextSize("9999").x;
        const float maxMonthGridItemWidth = dayNameSize.x > buttonSize.x ? dayNameSize.x : buttonSize.x;
        const float widthByMonthGrid = maxMonthGridItemWidth * 7.0f + style.ItemSpacing.x * 6.0f + style.WindowPadding.x * 2.0f;
        const float width = ImMax(ImMax(widthByTopLine, widthByComboText), widthByMonthGrid);//ImGui::CalcItemWidth();
        const float height =(buttonSize.y * 8.0f) + (style.WindowPadding.y * 2.0f) + style.ItemSpacing.y * 12.0f; //i counted 11 ItemSpacings with 2 separators. Seems to work despite 11 + 2 separators would be correct

        bool value_changed = false;

        ImGui::SetNextWindowSize(ImVec2(width, height));

        if (!ImGui::BeginCombo(label, currentDateText, 0)) return value_changed;

        if (g.IO.MouseClicked[1]) {
            // reset date when user right clicks the date chooser header whe the dialog is open
            d = GetCurrentDate();
            // move d at the first day of the month: mandatory fo the algo I'll use below
            d.tm_mday = 1;
            RecalculateDateDependentFields(d);  // now d.tm_wday is correct
        }
        else if (lastOpenComboID != id) {
            lastOpenComboID = id;
            if (dateOut.tm_mday == 0) dateOut = GetCurrentDate();
            d = dateOut;
            // move d at the first day of the month: mandatory fo the algo I'll use below
            d.tm_mday = 1;
            RecalculateDateDependentFields(d);  // now d.tm_wday is correct
        }

        //ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_PopupBg]);
        ImGui::Spacing();

        static const ImVec4 transparent(1, 1, 1, 0);
        ImGui::PushStyleColor(ImGuiCol_Button, transparent);

        static char yearString[12] = ""; sprintf_s(yearString, sizeof(yearString), "%d", 1900 + d.tm_year);
        const float yearPartWidth = arrowLeftWidth + arrowRightWidth + style.ItemSpacing.x * 2.0f + style.FramePadding.x * 4.0f + ImGui::CalcTextSize(yearString).x;

        const float oldWindowRounding = style.WindowRounding;
        style.WindowRounding = 0;

        ImGui::PushID(1234);
        ImGui::SetCursorPosX(style.WindowPadding.x);
        if (ImGui::SmallButton(arrowLeft)) { d.tm_mon -= 1; RecalculateDateDependentFields(d); }
        ImGui::SameLine();
        ImGui::Text("%s", monthNames[d.tm_mon]);
        ImGui::SameLine();
        if (ImGui::SmallButton(arrowRight)) { d.tm_mon += 1; RecalculateDateDependentFields(d); }
        ImGui::PopID();
        ImGui::SameLine(ImGui::GetWindowWidth() - yearPartWidth - style.WindowPadding.x);
        ImGui::PushID(1235);
        if (ImGui::SmallButton(arrowLeft)) { d.tm_year -= 1; if (d.tm_year < 0) d.tm_year = 0; RecalculateDateDependentFields(d); }
        ImGui::SameLine();
        ImGui::Text("%d", 1900 + d.tm_year);
        ImGui::SameLine();
        if (ImGui::SmallButton(arrowRight)) { d.tm_year += 1; RecalculateDateDependentFields(d); }
        ImGui::PopID();

        ImGui::Spacing();

        const static int numDaysPerMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        int maxDayOfCurMonth = numDaysPerMonth[d.tm_mon];   // This could be calculated only when needed (but I guess it's fast in any case...)
        if (maxDayOfCurMonth == 28) {
            const int year = d.tm_year + 1900;
            const bool bis = ((year % 4) == 0) && ((year % 100) != 0 || (year % 400) == 0);
            if (bis) maxDayOfCurMonth = 29;
        }

        ImGui::Separator();

        static char curDayStr[4] = "";
        static tm today = GetCurrentDate();
        float spacing = style.ItemSpacing.x + maxMonthGridItemWidth;
        int pickedDate = (d.tm_year == dateOut.tm_year) && (d.tm_mon == dateOut.tm_mon) ? dateOut.tm_mday : -1;
        int todayDate = (d.tm_year == today.tm_year) && (d.tm_mon == today.tm_mon) ? today.tm_mday : -1;

        float backup_padding_y = style.FramePadding.y;
        style.FramePadding.y = 0.0f;

        IM_ASSERT(d.tm_mday == 1);    // Otherwise the algo does not work
        // Display items
        ImGui::SetCursorPosX(style.WindowPadding.x);
        for (int dw = 0; dw < 7; dw++) {
            ImGui::BeginGroup();
            if (dw == 6) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.00f, 0.00f, 1.00f));
            ImGui::Text("%s", dayNames[dw]);
            ImGui::Separator();
            if (dw == 6) ImGui::Spacing(); //adds vertical shift for the specific column
            //-----------------------------------------------------------------------
            int mondayFirst = d.tm_wday == 0 ? 6 : d.tm_wday - 1; //Makes the Monday as the first day of the week. To make Sunday first: int curDay = dw - d.tm_wday
            int curDay = dw - mondayFirst;      // tm_wday is in [0,6]. For this to work here d must point to the first day of the month: i.e.: d.tm_mday = 1;
            for (int row = 0; row < 6; row++) {
                int cday = curDay + 7 * row;
                if (cday >= 0 && cday < maxDayOfCurMonth) {
                    ImGui::PushID(row * 1 + dw);
                    sprintf_s(curDayStr, sizeof(curDayStr), "%d", cday + 1);

                    bool isPickedDate = pickedDate == cday + 1;
                    bool isToday = todayDate == cday + 1;

                    if (isPickedDate) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 1.00f, 0.5f, 0.6f)); //Condition to highlight picked date button
                    if (isToday) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.00f, 1.0f, 1.0f)); //Condition to highlight today button
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.9f, 0.9f, 0.6f));
                    }
                    if (ImGui::Button(curDayStr, buttonSize)) {
                        value_changed = true;
                        SetItemDefaultFocus();
                        d.tm_mday = cday + 1;
                        RecalculateDateDependentFields(d);
                        dateOut = d;
                    }
                    if (isToday) ImGui::PopStyleColor(2);
                    if (isPickedDate) ImGui::PopStyleColor(); //Condition to undo the current date button style
                    ImGui::PopID();
                }
                else ImGui::TextUnformatted(" ");
            }
            if (dw == 6) ImGui::PopStyleColor();
            ImGui::EndGroup();
            //-----------------------------------------------------------------------   
            ImGui::SameLine();
            ImGui::SetCursorPosX((dw + 1) * spacing + style.WindowPadding.x);
        }
        style.FramePadding.y = backup_padding_y;
        style.WindowRounding = oldWindowRounding;
        ImGui::PopStyleColor();

        bool mustCloseCombo = value_changed;
        if (closeWhenMouseLeavesIt && !mustCloseCombo) {
            const float distance = g.FontSize * 1.75f;//1.3334f;//24;
            ImVec2 pos = ImGui::GetWindowPos(); pos.x -= distance; pos.y -= distance;
            ImVec2 size = ImGui::GetWindowSize(); size.x += 2.f * distance; size.y += 2.f * distance;
            const ImVec2& mousePos = ImGui::GetIO().MousePos;
            if (mousePos.x<pos.x || mousePos.y<pos.y || mousePos.x>pos.x + size.x || mousePos.y>pos.y + size.y) {
                mustCloseCombo = true;
            }
        }

        if (mustCloseCombo) ImGui::CloseCurrentPopup();
        ImGui::EndCombo();
        return value_changed;
    }

    void TestDateChooser(const char* dateFormat, bool closeWhenMouseLeavesIt, const char* leftArrow, const char* rightArrow, const char* upArrowString, const char* downArrowString) {
        static tm date = GetDateZero();
        if (DateChooser("DateChooser##id", date, dateFormat, closeWhenMouseLeavesIt, NULL, leftArrow, rightArrow, upArrowString, downArrowString)) {
            // Do something with date
        }
    }

} // namespace ImGui

// Adapt only the upstream panel host; capture, DSP and painting stay upstream.
// A layered, non-activating popup follows the desktop owner's stacking order.
void VizPanelBackdrop() {
    if (!g_overlayInPanel || !g_overlayWnd) return;
    LONG_PTR ex = GetWindowLongPtr(g_overlayWnd, GWL_EXSTYLE);
    ex |= WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW;
    if (g_settings.dragEnabled) ex &= ~WS_EX_TRANSPARENT;
    else ex |= WS_EX_TRANSPARENT;
    SetWindowLongPtr(g_overlayWnd, GWL_EXSTYLE, ex);
    SetLayeredWindowAttributes(g_overlayWnd, 0, 255, LWA_ALPHA);
    int backdrop = 1;  // DWMSBT_NONE
    int corners = 1;   // DWMWCP_DONOTROUND
    int border = -2;   // DWMWA_COLOR_NONE
    MARGINS margins{-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(g_overlayWnd, &margins);
    DwmSetWindowAttribute(g_overlayWnd, 38, &backdrop, sizeof(backdrop));
    DwmSetWindowAttribute(g_overlayWnd, 33, &corners, sizeof(corners));
    DwmSetWindowAttribute(g_overlayWnd, 34, &border, sizeof(border));
    SetWindowPos(g_overlayWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

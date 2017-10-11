package com.harry.engine;

import android.app.Activity;
import android.graphics.Rect;
import android.view.View;
import android.view.ViewTreeObserver;
import android.widget.FrameLayout;

/**
 * When an {@link Activity} is launched with
 * {@link android.view.WindowManager.LayoutParams#FLAG_FULLSCREEN}
 * ({@code android:windowFullScreen=true}) the
 * {@link android.view.WindowManager.LayoutParams#SOFT_INPUT_ADJUST_RESIZE}
 * ({@code android:windowSoftInputMode="adjustResize"}), by design, does
 * nothing. The expectation from Android is the full screen application
 * will adjust as needed. Unfortunately, Android doesn't automatically
 * notify the application of major resize events, such as when the soft
 * input method appears.
 * <p />
 * https://code.google.com/p/android/issues/detail?id=5497 was opened
 * against this problem but has been marked as 'Working As Intended'
 * so likely nothing will be done about it. This class serves as a
 * workaround by notifying {@link OnResizeListener} instances when the
 * first child of the {@link android.R.id#content} {@code View} changes
 * size.
 */
public class ViewSizeObserver {

    /**
     * Interface to be notified of dimension changes in the usable display.
     */
    public interface OnResizeListener {
        /**
         * Called when the dimensions of the usable display area have
         * changed.
         *
         * @param width currently usable width of the display area.
         * @param height currently usable height of the display area.
         */
        void onWindowResize(final int width, final int height);
    }

    /**
     * Start listening for window resize changes to {@code activity} and
     * reporting the changes to {@code listener}.
     *
     * @param activity whose view hierarchy will be observed.
     * @param listener to be notified on size changes.
     */
    public static ViewSizeObserver getInstance(final Activity activity, final OnResizeListener listener) {
        if (activity == null) {
            throw new IllegalArgumentException("Can't observe null Activity.");
        }
        return new ViewSizeObserver(activity, listener);
    }

    private final Rect currentBounds;
    private final Rect lastBounds;
    private final LayoutListener layoutListener;
    private final OnResizeListener onResizeListener;
    private final View rootChild;

    /**
     * Create a new instance for the view hierarchy of {@code activity}
     * that reports changes to {@code listener}.
     *
     * @param activity whose view hierarchy will be observed.
     * @param listener to be notified on size changes.
     */
    private ViewSizeObserver(final Activity activity, final OnResizeListener listener) {
        final FrameLayout content = (FrameLayout) activity.findViewById(android.R.id.content);
        this.layoutListener = new LayoutListener(this);
        this.rootChild = content.getChildAt(0);
        this.onResizeListener = listener;
        this.currentBounds = new Rect();
        this.lastBounds = new Rect();
    }

    /**
     * Begin observing resize changes. Triggers a call to
     * {@link OnResizeListener#onWindowResize(int, int)} immediately if
     * the dimensions of the view tree are different from the dimensions
     * of the screen.
     *
     * @return {@code this} for chaining.
     */
    public ViewSizeObserver start() {
        this.rootChild.getViewTreeObserver().addOnGlobalLayoutListener(this.layoutListener);
        // Set 'currentBounds' to the bounds of the root.
        // If currently different from max an onWindowResize call will trigger
        this.rootChild.getRootView().getWindowVisibleDisplayFrame(this.currentBounds);
        this.checkWindowDimensions();
        return this;
    }

    /**
     * Stop observing resize changes.
     *
     * @return {@code this} for chaining.
     */
    public ViewSizeObserver stop() {

      //  this.rootChild.getViewTreeObserver().removeOnGlobalLayoutListener(this.layoutListener);
        return this;
    }

    /**
     * Compute whether or not the view hierarchy has been resized. If
     * the window display bounds have changed report the current dimensions
     * to the {@link OnResizeListener}.
     */
    private void checkWindowDimensions() {
        this.lastBounds.set(this.currentBounds);
        this.rootChild.getWindowVisibleDisplayFrame(this.currentBounds);

        if (!this.currentBounds.equals(this.lastBounds) && this.onResizeListener != null) {
            this.onResizeListener.onWindowResize(this.currentBounds.width(), this.currentBounds.height());
        }
    }

    /**
     * {@link ViewTreeObserver.OnGlobalLayoutListener} implementation which
     * calls {@link #checkWindowDimensions()} in calls to
     * {@link ViewTreeObserver.OnGlobalLayoutListener#onGlobalLayout()}.
     */
    private static final class LayoutListener implements ViewTreeObserver.OnGlobalLayoutListener {
        private final ViewSizeObserver workaround;

        public LayoutListener(final ViewSizeObserver workaround) {
            this.workaround = workaround;
        }

        @Override
        public void onGlobalLayout() {
            this.workaround.checkWindowDimensions();
        }
    }

}
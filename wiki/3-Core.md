## 3.1 Description
The section called **core** is "a must to have" if you want to use any of the other sections. It creates a GUI window and handles all events, suchlike, mouse, keyboard and touch. Inside this class you will find a `Camera` object which you can handle and perform movements in the screen.

| Header | `#include "includes/console.h"` |
| --- | --- |
| cmake | includes/console.h <br/> src/console.h |


### 3.1.1 List of all members

<table>
  <tbody>
    <tr>
      <th>return</th>
      <th>member</th>
    </tr>
    <tr>
      <td>void</td>
      <td>[[camera_isometric_view|3-Core#337-change-view-to-top-view]]()</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[camera_rotate|3-Core#334-rotate-camera]](const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[camera_set_position|3-Core#331-setting-camera-position]](const float x = -12.0f, const float y = 0.0f, const float z = 5.0f)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[camera_set_target|3-Core#332-setting-camera-target-position]](const float x = 0.0f, const float y = 0.0f, const float z = 0.0f)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[camera_set_up|3-Core#333-setting-camera-upwards-direction]](const float x = 0.0f, const float y = 0.0f, const float z = 1.0f)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[camera_set_zoom|3-Core#336-setting-camera-zoom]](const float zoom = 1.0f)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[camera_top_view|3-Core#337-change-view-to-top-view]]()</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[camera_translate|3-Core#335-translate-camera]](const float x = 0.0f, const float y = 0.0f, const float z = 0.0f)</td>
    </tr>
    <tr>
      <td>int</td>
      <td>[[execute|3-core#329-execute-window]](const bool infinite_loop = true)</td>
    </tr>
    <tr>
      <td>int</td>
      <td>[[execute|3-core#329-execute-window]](const bool infinite_loop = true, const int width, const int height, const char *title = "Torero",
              const bool full_screen = true, const bool maximized = false)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[full_screen|3-core#326-full-screen-window]](const bool make_full = true)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[maximize_window|3-core#324-maximize-window]](const bool maximized = true)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[minimize_window|3-core#325-minimize-window]](const bool minimized = true)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[process_pending_events|3-core#328-processing-pending-events]]()</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[set_window_position|3-core#323-window-position]](const int x, const int y)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[set_window_size|3-core#322-window-size]](const int width, const int height)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[set_window_title|3-core#321-window-title]](const char *title)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[set_window_title|3-core#321-window-title]](const std::string title)</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[wait_for_events|3-core#327-wait-for-events]]()</td>
    </tr>
    <tr>
      <td>void</td>
      <td>[[wait_for_events|3-core#327-wait-for-events]](const double timeout)</td>
    </tr>
  </tbody>
</table>
<br/>

## 3.2 Window management

### 3.2.1 Window title

To change the window title (which appears at the bar in the top side of the window) use:
```c++
void set_window_title(const char *title);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const char*`] | **title** | The UTF-8 encoded window title. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.
___
Using standard string:
```c++
void set_window_title(const std::string title);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`std::string`] | **title** | The UTF-8 encoded window title. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.

___

### 3.2.2 Window size

This function changes the size of the window, use negative values to equalize window and monitor width or/and height. For full screen windows, the specified size becomes the new resolution of the window. The window is resized to fit the resolution of the set video mode.
```c++
void set_window_size(const int width, const int height);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const int`] | **width** | Size in pixels of the window. Use any negative value to set the window's width equal to the monitor's resolution. |
| [`const int`] | **height** | Size in pixels of the window. Use any negative value to set the window's height equal to the monitor's resolution. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.

___

### 3.2.3 Window position

Changes the window's position in screen, The origin of the coordinate system is at the top-left corner of the monitor.
```c++
void set_window_position(const int x, const int y);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const int`] | **x** | Position from the left side of the monitor in pixels. |
| [`const int`] | **y** | Position from the top side of the monitor in pixels |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.

___

### 3.2.4 Maximize window

Maximizes to fit all the monitor or resizes back the window to its defined width and height.
```c++
void maximize_window(const bool maximized = true);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const bool`] | **maximized** | Set this to `true` to maximize or `false` to restore the window size back to `width` and `height`. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.

___

### 3.2.5 Minimize window

Minimizes the window or resizes it back to its defined width and height.
```c++
void minimize_window(const bool minimized = true);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const bool`] | **minimized** | Set this to `true` to minimize or `false` to restore the window size back to `width` and `height`. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.

___

### 3.2.6 Full screen window

Makes the window full screen or resizes back the window to its defined width and height. **This will hide the title and launcher bar**.
```c++
void full_screen(const bool make_full = true);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const bool`] | **make_full** | Set this to `true` to make full screen or `false` to restore the window size back to `width` and `height`. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.

___

### 3.2.7 Wait for events

Waits until an event is triggered (mouse, keyboard, etc).
```c++
void wait_for_events();
```

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.
___
Waits until an event is triggered (mouse, keyboard, etc) or until the timeout (in seconds) has passed.
```c++
void wait_for_events(const double timeout);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const double`] | **timeout** | Time in seconds to wait if event is not triggered. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.

___

### 3.2.8 Processing pending events

Processes all pending events.
```c++
void process_pending_events();
```

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return error if the window was not created properly.

___

### 3.2.9 Execute window

This is the main function; it will open a new window with predefined properties or properties defined by the user suchlike, width, height, title, etc. If `infinite_loop = true` then, it will maintain the window open until a close event is triggered.
```c++
int execute(const bool infinite_loop = true);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const bool`] | **infinite_loop** | if set to `true` it will maintain the window open until close event. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return the type of error if the window was not created properly; `EXIT_SUCCESS`, `GLFW_NOT_LOADED`, `WINDOW_NOT_LOADED`, `GLAD_NOT_LOADED` or `EXISTING_WINDOW`.
___
This also configures several values at the same time:
```c++
int execute(const bool infinite_loop = true, const int width, const int height,
            const char *title = "Torero", const bool full_screen = true,
            const bool maximized = false
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const bool`] | **infinite_loop** | if set to `true` it will maintain the window open until close event. |
| [`const int`] | **width** | Window's width in pixels. |
| [`const int`] | **height** | Window's height in pixels. |
| [`const char*`] | **title** | The UTF-8 encoded window's title. |
| [`const bool`] | **full_screen** | if set to `true` it will make the window full screen. |
| [`const bool`] | **maximized** | if set to `true` it will maximize the window, ignored if `full_screen = true`. |

 &nbsp; **Errors**<br />
 &nbsp; &nbsp; This will return the type of error if the window was not created properly; `EXIT_SUCCESS`, `GLFW_NOT_LOADED`, `WINDOW_NOT_LOADED`, `GLAD_NOT_LOADED` or `EXISTING_WINDOW`.
<br/><br/>
## 3.3 Camera management

### 3.3.1 Setting camera position

Changes the position of the camera, note that is only the location of the camera, the point that the camera is aiming will not be modified.
```c++
void camera_set_position(const float x = -12.0f, const float y = 0.0f, const float z = 5.0f);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const float`] | **x** | Position in meters in the X axis. |
| [`const float`] | **y** | Position in meters in the Y axis. |
| [`const float`] | **z** | Position in meters in the Z axis. |

___

### 3.3.2 Setting camera target position

This specifies the point which the camera is looking.
```c++
void camera_set_target(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const float`] | **x** | Position in meters in the X axis. |
| [`const float`] | **y** | Position in meters in the Y axis. |
| [`const float`] | **z** | Position in meters in the Z axis. |

___

### 3.3.3 Setting camera upwards direction

Changes the rotation of the camera, changes the upward side of the camera. You must introduce the coordinates of a **normalized** vector, this vector represents the camera orientation, by default is pointing up parallel to Y axis.
```c++
void camera_set_up(const float x = 0.0f, const float y = 0.0f, const float z = 1.0f);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const float`] | **x** | Position in meters in the X axis, must be normalized. |
| [`const float`] | **y** | Position in meters in the Y axis, must be normalized. |
| [`const float`] | **z** | Position in meters in the Z axis, must be normalized. |

___

### 3.3.4 Rotate camera

Multiplies the Camera position by a rotation matrix using euler angles (the center is the camera target's position), the angles are in **RADIANS**.
```c++
void camera_rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const float`] | **pitch** | Pitch angle in radians. |
| [`const float`] | **yaw** | Yaw angle in radians. |
| [`const float`] | **roll** | Roll angle in radians. |

___

### 3.3.5 Translate camera

Multiplies the Camera position by a translation matrix (the center is the camera target's position).
```c++
void camera_translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const float`] | **x** | Distance in X axis to translate. |
| [`const float`] | **y** | Distance in Y axis to translate. |
| [`const float`] | **z** | Distance in Z axis to translate. |

___

### 3.3.6 Setting camera zoom

This changes the scenary zoom, maximum = 0.05 : minimum = 15. The smaller the zoom factor is, the closer to the target the camera position becomes.
```c++
void camera_set_zoom(const float zoom = 1.0f);
```
 &nbsp; **Parameters**

| Type | Name | Description |
| --- | --- | --- |
| [`const float`] | **zoom** | Zoom factor, if `zoom = 0.05` then, the camera position **vector** is multiplied for 0.05 |

___

### 3.3.7 Change view to top view

Changes the view to **Top view**.
```c++
void camera_top_view();
```

___

### 3.3.8 Change view to isometric view

Changes the view to **Isometric view**.
```c++
void camera_isometric_view();
```
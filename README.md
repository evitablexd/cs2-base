This is a simple "base", all you need to do is add scheme system/game sdk :)

It contains:
- Shadow VMT Hooking (undetected)
- ImGui Menu
- FrameStageNotify hook example

Flaws:
- Calling CreateInterface directly
- Ugly dx11 swap chain creation code

Pre-requisites:
- Premake 5
- VS2019+

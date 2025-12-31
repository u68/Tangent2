cmake_minimum_required(VERSION 3.20)
project(TangentSDK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Discord RPC library (built from source)
set(DISCORD_RPC_LIB "${CMAKE_SOURCE_DIR}/libdiscord-rpc.a")

add_executable(TangentSDK
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/EditorWidget.cpp
    src/ui/TabEditorWidget.cpp
    src/ui/ConsoleWidget.cpp
    src/ui/ProjectExplorer.cpp
    src/ui/TitleBarWidget.cpp
    src/ui/SettingsDialog.cpp
    src/editor/SyntaxHighlighter.cpp
    src/editor/CompletionPopup.cpp
    src/app/DiscordRPCManager.cpp
    src/theme/SyntaxTheme.cpp
)

target_link_libraries(TangentSDK Qt6::Widgets ${DISCORD_RPC_LIB})

set(TEST_CASES FileSystemTest InputEventTest SceneLoadTest SceneRenderTest BasicPhysicsTest EditorCameraTest
        )

foreach(TEST_CASE IN LISTS TEST_CASES)
    add_executable(${TEST_CASE} ${TEST_CASE}.cpp)
    target_link_libraries(${TEST_CASE} Vkita)
    add_test(NAME TEST_${TEST_CASE} COMMAND ${TEST_CASE})
endforeach(TEST_CASE)

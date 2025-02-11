add_executable(hello1 hello1.cpp)
add_executable(hello2 hello2.cpp)
add_executable(hello3 hello3.cpp)
add_executable(hello4 hello4.cpp)

add_executable(counter counter.cpp)

add_executable(mutex1 mutex1.cpp)
add_executable(mutex2 mutex2.cpp)
add_executable(mutex3 mutex3.cpp)
add_executable(mutex4 mutex4.cpp)

add_executable(cv1 cv1.cpp)
add_executable(cv2 cv2.cpp)
add_executable(cv3_timed cv3_timed.cpp)

add_executable(bounded_buffer bounded_buffer.cpp)

#add_executable(semaphore_win32 semaphore_win32.cpp)
add_executable(semaphore semaphore.cpp)

#add_executable(rwlock1 rwlock1.cpp)

# upgrade_lock is Boost only.
# add_executable(rwlock2_upgrade rwlock2_upgrade.cpp)
target_link_libraries(hello1 PRIVATE Threads::Threads)
target_link_libraries(hello2 PRIVATE Threads::Threads)
target_link_libraries(hello3 PRIVATE Threads::Threads)
target_link_libraries(hello4 PRIVATE Threads::Threads)
target_link_libraries(mutex1 PRIVATE Threads::Threads)
target_link_libraries(mutex2 PRIVATE Threads::Threads)
target_link_libraries(mutex3 PRIVATE Threads::Threads)
target_link_libraries(mutex4 PRIVATE Threads::Threads)
target_link_libraries(counter PRIVATE Threads::Threads)
target_link_libraries(cv2 PRIVATE Threads::Threads)
target_link_libraries(cv3_timed PRIVATE Threads::Threads)
target_link_libraries(bounded_buffer PRIVATE Threads::Threads)
target_link_libraries(semaphore PRIVATE Threads::Threads)
target_link_libraries(cv1 PRIVATE Threads::Threads)


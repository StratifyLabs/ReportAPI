
set(DEPENDENCIES TestAPI FsAPI JsonAPI ReportAPI)

api_add_test_executable(ReportAPI 32768 "${DEPENDENCIES}")




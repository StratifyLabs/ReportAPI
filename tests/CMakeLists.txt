
set(DEPENDENCIES TestAPI FsAPI JsonAPI ReportAPI)

stratify_api_add_test_executable(ReportAPI 32768 "${DEPENDENCIES}")




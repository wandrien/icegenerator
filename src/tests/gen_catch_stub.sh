MODULE_NAME="$1"

if [ "$MODULE_NAME" = main.cpp ] ; then
  CATCH_MAIN="#define CATCH_CONFIG_MAIN"
else
  CATCH_MAIN=""
fi

UNIT_FILE_NAME="catch_${MODULE_NAME}"
if [ -f "$UNIT_FILE_NAME" ] ; then
  UNIT_FILE_INCLUDE="#include \"${UNIT_FILE_NAME}\""
else
  UNIT_FILE_INCLUDE=""
fi

echo "
#define INSIDE_CATCH
${CATCH_MAIN}
#include \"catch.hpp\"
#include \"../${MODULE_NAME}\"
${UNIT_FILE_INCLUDE}
"

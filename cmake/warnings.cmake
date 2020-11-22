if(MSVC)
  set(
      project_warnings
      /W4 # Set warning level 4 (/Wall is too noisy)
      /WX # Warnings as errors
      # Disabled warnings
      /wd4459 # declaration of 'xs' hides global declaration
      /wd4127 # conditional expression is constant
      /wd4100 # unreferenced formal parameter
      /wd4189 # local variable is initialized but not referenced
      /wd4244 # '=': conversion from 'unsigned int' to 'char', possible loss of data
      # Additional warnings
      /we4263 # member function does not override any base class virtual member function)
      /we4264 # no override available for virtual member function; function is hidden
      /we4242 # conversion from 'type1' to 'type2', possible loss of data
      /we4266 # no override available for virtual member function from base 'type'; function is hidden
      /we4302 # truncation from 'type 1' to 'type 2'
      /we4826 # conversion from 'type1' to 'type2' is sign-extended. This may cause unexpected runtime behavior
      /we4905 # wide string literal cast to 'LPSTR'
      /we4906 # string literal cast to 'LPWSTR'
      /we4389 # signed/unsigned mismatch
      /we4239 # nonstandard extension used: 'argument': conversion from T to T &
      /we4928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
      /we4505 # unreferenced local function has been removed
      /we4265 # 'class': class has virtual functions, but destructor is not virtual
      /we4191 # unsafe conversion from 'type of expression' to 'type required'
      /we4456 # declaration of variable hides previous local declaration
      /we4458 # declaration of variable hides class member
      # /we4711 # The compiler performed inlining on the given function, although it was not marked for inlining
  )
else()
  set(
      project_warnings
      -Wall
      -Wextra
      -pedantic
      -Werror
      -Weffc++
      -Wconversion
      -Wsign-conversion
      -Wctor-dtor-privacy
      -Wreorder
      -Wold-style-cast
      -Wparentheses
  )
endif()

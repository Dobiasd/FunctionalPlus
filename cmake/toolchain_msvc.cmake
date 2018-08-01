set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set warning level 4 (the most elevated level is /Wall but it is too noisy)
add_compile_options("/W4")
# Warnings as errors
add_compile_options("/WX")

set(msvc_disabled_warnings
  4459 # declaration of 'xs' hides global declaration
  4127 # conditional expression is constant
  4100 # unreferenced formal parameter
  4189 # local variable is initialized but not referenced
  4244 # '=': conversion from 'unsigned int' to 'char', possible loss of data
)
foreach(warningNumber ${msvc_disabled_warnings})  
    add_compile_options(/wd${warningNumber})
endforeach()

# Additional warnings on top of the level 4 warnings
set(msvc_additional_warnings 
    4263 # member function does not override any base class virtual member function) 
    4264 # no override available for virtual member function; function is hidden
    4242 # conversion from ‘type1’ to ‘type2’, possible loss of data
    4266 # no override available for virtual member function from base ‘type’; function is hidden
    4302 # truncation from ‘type 1’ to ‘type 2’
    4826 # conversion from ‘type1’ to ‘type2’ is sign-extended. This may cause unexpected runtime behavior
    4905 # wide string literal cast to ‘LPSTR’
    4906 # string literal cast to ‘LPWSTR’
    4389 # signed/unsigned mismatch
    4239 # nonstandard extension used: 'argument': conversion from T to T &
    4928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
    4505 # unreferenced local function has been removed
    4265 # 'class': class has virtual functions, but destructor is not virtual
    4191 # unsafe conversion from ‘type of expression’ to ‘type required’ 
    4456 # declaration of variable hides previous local declaration
    4458 # declaration of variable hides class member
    # 4711 # The compiler performed inlining on the given function, although it was not marked for inlining
)
foreach(warningNumber ${msvc_additional_warnings})  
    add_compile_options(/we${warningNumber})
endforeach()

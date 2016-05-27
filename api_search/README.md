todo:

equality (db == query):
VariableType _ == TypeConstructor _
TypeApplication (TypeConstructor "Maybe") (VariableType x) == x
TypeApplication (TypeConstructor "Maybe") (TypeConstructor x) == x

search signature with query as subSignature (both normalized)

hoogle talk slides:
http://ndmitchell.com/downloads/slides-hoogle-08_dec_2005.pdf
http://ndmitchell.com/downloads/slides-hoogle_fast_type_searching-09_aug_2008.pdf
http://ndmitchell.com/downloads/paper-hoogle_overview-19_nov_2008.pdf
http://ndmitchell.com/downloads/slides-hoogle_finding_functions_from_types-16_may_2011.pdf

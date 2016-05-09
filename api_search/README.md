haskell type in die comments für FunctionalPlus API search engine, filter : (a -> bool), [a] -> [a] oder auch [a], (a -> bool) -> [a]

todo:
1) komma bindet enger als pfeil
2) pfeil kann immer auch gefunden werden wenn man nach komma sucht
3) case insensitive suchen
4) signaturen mit query als teil suchen
5) int auch auf size_t in queries matchen
6) a, b usw. auf beliebige typen matchen
7) [a], [b] usw. auf string matchen


http://ndmitchell.com/downloads/slides-hoogle-08_dec_2005.pdf
http://ndmitchell.com/downloads/slides-hoogle_fast_type_searching-09_aug_2008.pdf
http://ndmitchell.com/downloads/paper-hoogle_overview-19_nov_2008.pdf
http://ndmitchell.com/downloads/slides-hoogle_finding_functions_from_types-16_may_2011.pdf


normalisierung:
alles klein
alle leerzeichen weg
komma durch pfeil ersetzen
nach words splitten
alle klammern weg
words alphabetisch ersetzen, aber nur wenn sie nur einen buchstaben lang sind: LUT = zip (nub xs) [a..z]

query muss substring von db-entry-type sein

oder doch lieber parsen
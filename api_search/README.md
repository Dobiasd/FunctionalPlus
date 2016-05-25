haskell type in die comments für FunctionalPlus API search engine, filter : (a -> bool), [a] -> [a] oder auch [a], (a -> bool) -> [a]

todo:
- case insensitive suchen
- signaturen mit query als teil suchen
- int auch auf size_t in queries matchen
- a, b usw. auf beliebige typen matchen
- [a], [b] usw. auf string matchen
- typen umwandeln:
    Char == a
    String == [a]
    [Char] == String
    Double = Float

- wenn das query geparsed werden kann, das irgendwo anzeigen

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
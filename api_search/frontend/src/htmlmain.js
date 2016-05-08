function init() {
  var mainDiv = document.getElementById('main');
  elmContent = Elm.embed(Elm.FPlusApiSearch, mainDiv, {});
  setTimeout(function() {
    console.log("Hello");
    console.log(hljs.listLanguages());
    hljs.initHighlighting.called = false;
    hljs.initHighlighting();
  }, 1000);
}
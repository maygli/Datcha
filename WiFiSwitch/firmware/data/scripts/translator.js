class Translator{
    static translate(theLang, theRoot){
        window["i18next"].changeLanguage(theLang).then((t) => {
            let aNeedTranslation = theRoot.querySelectorAll("[data-i18n]");
            for( let i = 0 ; i < aNeedTranslation.length ; i++ ){
                let aTransEl = aNeedTranslation[i];
                let aKey = aTransEl.getAttribute("data-i18n");
                let aVal = t(aKey);
                aTransEl.innerHTML = aVal;
            }
        });
    }        
}

export {Translator};
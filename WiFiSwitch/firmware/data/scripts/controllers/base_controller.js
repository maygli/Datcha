class BaseController{
    constructor(theEl){
        this._m_BaseElement = theEl;
        this._m_DisplayStyle = this._m_BaseElement.style.display;
    }

    show(){
        this._m_BaseElement.style.display = this._m_DisplayStyle;
    }

    hide(){
        this._m_BaseElement.style.display = "none";
    }

}

export {BaseController};
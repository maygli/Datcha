import {BaseController} from "./base_controller.js"

class PasswordController extends BaseController{
    constructor(theElement){
        super(theElement);
        this._m_Button = this._m_BaseElement.querySelector("img");
        this._m_Button.onclick = this._onButtonClick.bind(this);
        this._m_Input = this._m_BaseElement.querySelector("input");
    }

    _onButtonClick(){
        let aType = this._m_Input.getAttribute("type");
        if( aType == "text" ){
            this._m_Input.setAttribute("type", "password");
            this._m_Button.setAttribute("src","images/eye.png")
        }
        if( aType == "password" ){
            this._m_Input.setAttribute("type", "text");
            this._m_Button.setAttribute("src","images/close_eye.png")
        }        
    }
}

export {PasswordController};
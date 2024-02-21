import {Translator} from "../translator.js"
import {HttpProcessor} from "../http_processor.js"
import {BaseController} from "./base_controller.js"

class PageController extends BaseController{
    constructor(theContentEl){
        super(theContentEl);
        this._m_IsLoaded = false;
        this._m_URL = this._m_BaseElement.getAttribute("data-controller_url");
    }

    async _load(){
        let aData = await HttpProcessor.loadData(this._m_URL, true);
        this._m_BaseElement.innerHTML = aData;
        await this._upadteAfterLoad();
    }

    async _upadteAfterLoad(){
        Translator.translate(i18next.language, this._m_BaseElement);
        this._m_IsLoaded = true;
    }

    async show(){
        super.show()
        if( this._m_IsLoaded == false){
            await this._load();
        }
    }
}

class ParametersPageController extends PageController{
    constructor(theContentEl){
        super(theContentEl);
        this._m_ParamReq = this._m_BaseElement.getAttribute("data-controller_request");;
    }

    async setPageParameters(theData){

    }

    async _load(){
        await super._load();
        let aData = await HttpProcessor.loadData(this._m_ParamReq,false);
        await this.setPageParameters(aData);
    }
}

class FormPageController extends ParametersPageController{
    constructor(theContentEl){
        super(theContentEl);
    }

    async setPageParameters(theData){
        const { elements } = this._m_BaseElement.querySelector('form')

        for (const [ key, value ] of Object.entries(theData) ) {
            const field = elements.namedItem(key)
            if( field ){
                if( field.type == "checkbox"){
                    if( field.value == value )
                        field.checked = true;
                    else
                        field.checked = false;
                }
                else{
                    field.value = value;
                }
            }
        }

        let aForm = this._m_BaseElement.querySelector("form");
        aForm.onsubmit = this.submit.bind(this);
    }

    async submit(event){
        event.preventDefault();
        let aStatusEl = this._m_BaseElement.querySelector("#submit_response");
        let aForm = this._m_BaseElement.querySelector("form");
        let anURL = aForm.getAttribute("action");
        try{
            let aResponse = await HttpProcessor.sendFormData(anURL,aForm);
            if( aStatusEl ){
                aStatusEl.className = "server_message_text send_success";
                aStatusEl.innerHTML = i18next.t("submit_suceess");
            }
    
        }
        catch(anError){
            if( aStatusEl ){
                aStatusEl.className = "server_message_text send_fail";
                aStatusEl.innerHTML = i18next.t("submit_fail");
            }
        }
        if( aStatusEl ){
            aStatusEl.style.display = "inline-block";
            setTimeout(this._onHideServerMsg.bind(this), 2000);
        }
    }

    _onHideServerMsg()
    {
        let anEl = this._m_BaseElement.querySelector("#submit_response");
        if( anEl ){
            anEl.style.display = "none";
    }
}

}

export {PageController,ParametersPageController,FormPageController};
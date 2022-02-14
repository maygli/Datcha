import {PageController} from "./controllers/page_controller.js"
import {ExpandableController} from "./controllers/expandable_controller.js"
import {HttpProcessor} from "./http_processor.js"
import {UploadDialogController} from "./controllers/upload_dialog_controller.js"

class UpgradeController extends PageController{
    constructor(theEl){
        super(theEl);
    }

    async _upadteAfterLoad(){
        super._upadteAfterLoad();
        let aModal = this._m_BaseElement.querySelector('[data-controller_class="UploadDialogController"]');
        this._m_UploadDialogController = new UploadDialogController(aModal);
        this._m_UploadDialogController.hide();
        this._m_DescContent = this._m_BaseElement.querySelector('[data-controller_class="ExpandableController"]');
        this._m_ExpandableController = new ExpandableController(this._m_DescContent);
        this._m_UploadButton =  this._m_BaseElement.querySelector('[data-controller_item="UploadButton"]');
        this._m_UploadButton.addEventListener("change", this._onUploadFile.bind(this)); 
        this._m_IsLoaded = true;
     }

    _onUploadFile(){
        let aFile = document.getElementById("file").files[0];
        document.getElementById("file").value="";
        this._m_UploadDialogController.startUpload(aFile.name);
        if( aFile.name.length > 31 ){
            this._m_UploadDialogController.setError(i18next.t("settings_upgrade_name_exceed"));
            return false;
        }
        let aParFormData = new FormData();
        let aClear = document.getElementById("clear").checked;
        aParFormData.append("clear", aClear);
    
        HttpProcessor.uploadFile(aFile, "internal_upload", aParFormData, 
                                this._onLoadProgress.bind(this),
                                this._onLoadComplete.bind(this),
                                this._onLoadError.bind(this),
                                this._onLoadAbort.bind(this)
                                );
    }

    _onLoadProgress(event) {
        this._m_UploadDialogController.setProgress(event.loaded, event.total);
    }
    
    _onLoadComplete(event) {
        if(  event.target.status >= 200 && event.target.status < 300 ){
            this._m_UploadDialogController.setSuccess(event.target.responseText);
        }
        else{
            this._m_UploadDialogController.setError(event.target.responseText);
        }
    }
    
    _onLoadError(event) {
        this._m_UploadDialogController.setError(i18next.t("settings_upgrade_failed"));
    }
    
    _onLoadAbort(event) {
        this._m_UploadDialogController.setError(i18next.t("settings_upgrade_aborted"));
    }
}

export {UpgradeController};
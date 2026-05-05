var urlJsonCommon = {};

/** HTML转义 */ function escapeHTML(str) {
    str += "";
    return str.replace(/&/g, "&amp;").replace(/</g, "&lt; ").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&apos;");
}

/** HTML解码 */ function unescapeHTML(str) {
    str += "";
    return str.replace(/&lt;/g, "<").replace(/&gt;/g, ">").replace(/&amp;/g, "&").replace(/&quot;/g, '"').replace(/&apos;/g, "'");
}

/** 校验url @param url @returns {*} */ function checkXssUrl(url) {
    if (url) {
        url = url.replace(/\s/g, "");
        var illeagalVal = ["alert(", "eval(", "prompt(", "script:", "window.", "location.", "confirm("];
        for (var i = 0; i < illeagalVal.length; i++) {
            if (url.indexOf(illeagalVal[i]) > -1) {
                url = "";
                break;
            }
        }
    }
    return url;
}

/** url中文解码-特殊转义 */ function decodeURIComponentNew(searchText) {
    searchText = searchText.replace('%', '%25');/*%特殊转义*/
    searchText = decodeURIComponent(searchText);
    return searchText;
}

/** loading */ !(function (window) {
    if (window.LoadUI && window.LoadUI.showLoading) {
        return;
    }
    var commonPath = $('#common-chang-path').val();
    var loadingGifUrl = commonPath + "/pages/commonFile/img/loading1.gif";
    var loadingDiv = document.createElement("div");
    loadingDiv.id = "loadUILayer";
    loadingDiv.style.position = "fixed";
    loadingDiv.style.left = 0;
    loadingDiv.style.right = 0;
    loadingDiv.style.top = 0;
    loadingDiv.style.bottom = 0;
    loadingDiv.style.background = "rgba(255,255,255,0)";
    loadingDiv.style.zIndex = 99999;
    loadingDiv.style.display = "none";
    var loadGif = document.createElement("img");
    loadGif.src = loadingGifUrl;
    loadGif.style.margin = "auto";
    loadGif.style.position = "absolute";
    loadGif.style.left = 0;
    loadGif.style.right = 0;
    loadGif.style.top = 0;
    loadGif.style.bottom = 0;
    loadGif.style.width = "40px";
    loadGif.style.height = "40px";
    loadingDiv.appendChild(loadGif);
    if (document.body) {
        document.body.appendChild(loadingDiv);
    } else {
        window.addEventListener("DOMContentLoaded", function () {
            document.body.appendChild(loadingDiv);
        })
    }
    var num = 0;
    var loadUI = {};
    window.LoadUI = loadUI;
    LoadUI.showLoading = function () {
        num++;
        loadingDiv.style.display = "block";
    };
    LoadUI.hideLoading = function () {
        num--;
        num = num < 0 ? 0 : num;
        if (num < 1) loadingDiv.style.display = "none";
    };
})(window);
/** popwindow */ !(function ($) {/*定义一个构造函数*/
    function InitHardDialogBox(elem, opt) {
        var defaultOpt = {};
        this.element = $(elem);
        this.options = $.extend(defaultOpt, opt);
        var lang = this.options.langs;
        this.initShow();
        this.closeIcon();
        this.sureBtnInit();
        this.cancleInit();
    }/*增加构造函数的方法*/
    InitHardDialogBox.prototype = {/* 初始化*/ initShow: function () {
            let _this = this;
            $("#common-reLogin-pop .common-dialog-box").fadeIn(50);
            $("#common-reLogin-pop .tips-dialog-content").text(_this.options.messages);
            if (_this.options.isCancel == "Y") {
                $("#common-reLogin-pop .tips-dialog-footer .cancel-btn").hide();
            } else {
                $("#common-reLogin-pop .tips-dialog-footer .cancel-btn").show();
            }
        },/* 确认*/ sureBtnInit: function () {
            let _this = this;
            $("#common-reLogin-pop .sure-btn").off("click").on("click", function (e) {
                stopBubble(e);
                $("#common-reLogin-pop .common-dialog-box").fadeOut(100);
                _this.options.onDialog(true);
            })
        },/* 取消事件*/ cancleInit: function () {
            let _this = this;
            $("#common-reLogin-pop .cancel-btn").off("click").on("click", function (e) {
                stopBubble(e);
                $("#common-reLogin-pop .common-dialog-box").fadeOut(100);
                _this.options.onDialog(false);
            })
        },/* 关闭icon*/ closeIcon: function () {
            let _this = this;
            $("#common-reLogin-pop .selectBox-head-icon").off("click").on("click", function (e) {
                stopBubble(e);
                $("#common-reLogin-pop .common-dialog-box").fadeOut(100);
                _this.options.onDialog(false);
            })
        }
    };
    $.fn.hardDialogBox = function (options) {
        new InitHardDialogBox(this, options);
    };
})(jQuery);
/** diaTip */ !(function ($) {
    var timer;/*定义一个构造函数*/
    function InitDialogTip(elem, opt) {
        var defaultOpt = {
            idSelector: '',/*选择器*/
            lang: $('#lang').val(),
            isTitle: true,/*是否有标题*/
            title: $('#dialog-title').val(),/*头部标题*/
            message: '',/*提示内容*/
            isCancel: true,/*是否保留取消按钮*/
            isClose: false,/*关闭图标*/
            isSure: true,/*是否保留确定按钮*/
            cancelTxt: $('#dialog-btn-close').val(),/*取消按钮名称*/
            sureTxt: '',/*确定按钮名称*/
            isTime: true,/*是否开启倒计时*/
            timeBefore: $('#dialog-time-before').val(),/*倒计时提示文字*/
            timeAfter: $('#dialog-time-after').val(),/*倒计时提示文字*/
            timeNum: 9,/*倒计时多久自动关闭*/
            onSure: function () {
            },/*确认按钮绑定事件*/
            onCancel: function () {
            },/*关闭按钮绑定事件*/
        };
        this.element = $(elem);
        this.options = $.extend(defaultOpt, opt);
        this.initShow();
        this.sureBtnInit();
        this.cancelInit();
    }/*增加构造函数的方法*/
    InitDialogTip.prototype = {/* 初始化*/ initShow: function () {
            this.element.empty();
            let str = '';
            if (this.options.isTitle) {
                str += '<div class="title"><i class="warning"></i>' + this.options.title + '</div>';
            }
            if (this.options.isClose) {
                str += '<i class="close"></i>';
            }
            if (this.options.message) {
                str += '<div class="content">' + this.options.message + '</div>';
            }
            if (this.options.isSure || this.options.isCancel || this.options.isTime) {
                str += '<div class="foot">';
                if (this.options.isTime) {
                    str += '<div class="time">' + this.options.timeBefore + '<span>' + this.options.timeNum + '</span>' + this.options.timeAfter + '</div>'
                }
                if (this.options.isSure || this.options.isCancel) {
                    str += '<div class="btn-group">';
                    if (this.options.isSure) {
                        str += '<a class="sure-btn common-btn">' + this.options.sureTxt + '</a>'
                    }
                    if (this.options.isCancel) {
                        str += '<a class="cancel-btn common-btn">' + this.options.cancelTxt + '</a>'
                    }
                    str += '</div>'
                }
                str += '</div>'
            }
            this.element.append(str);
            this.element.show();
            if (this.options.isTime) {
                this.timeInit();
            } else {
                this.element.find('.foot').addClass('noTime');
            }
            if (this.options.lang != 'zh' && this.options.isTime) {
                this.element.find('.content').addClass('enContent');
                this.element.find('.foot').addClass('enFoot');
            }
        },/* 确认*/ sureBtnInit: function () {
            let _this = this;
            this.element.find(".sure-btn").off("click").on("click", function (e) {
                stopBubble(e);
                _this.element.css({visibility: 'hidden', opacity: 0});
                if (_this.options.isTime) {
                    clearInterval(timer);
                }
                _this.options.onSure();
            })
        },/* 取消事件*/ cancelInit: function () {
            let _this = this;
            this.element.find(".cancel-btn").off("click").on("click", function (e) {
                stopBubble(e);
                _this.element.css({visibility: 'hidden', opacity: 0});
                if (_this.options.isTime) {
                    clearInterval(timer);
                }
                _this.options.onCancel();
            });
            this.element.find("i.close").off("click").on("click", function (e) {
                stopBubble(e);
                _this.element.css({visibility: 'hidden', opacity: 0});
                _this.options.onCancel();
            });
        },/*倒计时关闭*/ timeInit: function () {
            let num = this.options.timeNum;
            let _this = this;
            timer = setInterval(function () {
                if (num == 0) {
                    _this.element.css({visibility: 'hidden', opacity: 0});
                    clearInterval(timer);
                    _this.options.onCancel();
                    return;
                }
                num--;
                _this.options.timeNum = num;
                _this.element.find('.time>span').text(num);
            }, 1000);
        }
    };
    $.fn.dialogTip = function (options) {
        new InitDialogTip(this, options);
    };
})(jQuery);

/** highlight */ function replaceSpecialWord(flag) {
    flag = flag.replace(/\\/g, '\\\\').replace(/\(/g, '\\(').replace(/\[/g, '\\[').replace(/\)/g, '\\)').replace(/\]/g, '\\]').replace(/\*/g, '\\*').replace(/\./g, '\\.').replace(/\?/g, '\\?').replace(/\{/g, '\\{').replace(/\}/g, '\\}').replace(/\|/g, '\\|').replace(/\$/g, '\\$').replace(/\^/g, '\\^').replace(/\+/g, '\\+').replace(/\'/g, "\\'").replace(/\．/g, "\\．").replace(/\%/g, "\\%");
    return flag;
}

function replaceAll(src, oldStr, newStr) {
    if (typeof (src) != "string") {
        return src;
    }
    return src.replace(new RegExp(oldStr, 'ig'), newStr);
}

function highLight(o, flag, divideFlag, rndColor, url) {
    var bgCor = '';
    var fgCor = '';
    if (rndColor) {
        bgCor = fRndCor(10, 20); /*获取背景色*/
        fgCor = fRndCor(230, 255); /*获取字体颜色*/
    } else {
        bgCor = '#FFFF00';
        fgCor = 'black';
    }
    for (var i = 0; i < o.childNodes.length; i++) {
        var o_ = o.childNodes[i]; /*获取当前子节点*/
        var o_p = o_.parentNode; /*获取父节点*/
        if (o_.nodeType == 1) { /*节点值为1是非叶子节点*/
            highLight(o_, flag, divideFlag, rndColor, url);/*价格1判断是否已经替换特殊字符*/
        } else if (o_.nodeType == 3) { /*节点值3为text*/
            var newFlag = replaceSpecialWord(flag);/*只有当产品联想时才进行分词替换*/
            if (!divideFlag) {
                newFlag = replaceAll(newFlag, ' ', '\|');
            }
            var re = new RegExp(newFlag, 'gi');
            if (o_.data.search(re) == -1) continue;
            var temp = fEleA(o_.data, newFlag, '', bgCor);
            o_p.replaceChild(temp, o_);
        }
    }

    /** 替换特殊字符 @param flag @returns */ function replaceWord(flag) {
        flag = flag.replace(/\\/g, '\\\\').replace(/\(/g, '\\(').replace(/\[/g, '\\[').replace(/\)/g, '\\)').replace(/\]/g, '\\]').replace(/\*/g, '\\*').replace(/\./g, '\\.').replace(/\?/g, '\\?').replace(/\{/g, '\\{').replace(/\}/g, '\\}').replace(/\|/g, '\\|').replace(/\$/g, '\\$').replace(/\^/g, '\\^').replace(/\+/g, '\\+').replace(/\'/g, "\\'").replace(/\．/g, "\\．");
        return flag.toString();
    }

    /** 获取rgb颜色值 */ function fRndCor(under, over) {
        if (arguments.length == 1) {
            var over = under;
            under = 0;
        } else if (arguments.length == 0) {
            var under = 0;
            var over = 255;
        }
        var r = fRandomBy(under, over).toString(16);
        r = padNum(r, r, 2);
        var g = fRandomBy(under, over).toString(16);
        g = padNum(g, g, 2);
        var b = fRandomBy(under, over).toString(16);
        b = padNum(b, b, 2);
        return '#' + r + g + b;

        /** 将字符转化为数字， @param under @param over @returns */ function fRandomBy(under, over) {
            switch (arguments.length) {
                case 1:
                    return parseInt(under + 1);
                case 2:
                    return parseInt((over - under + 1) + under);
                default:
                    return 0;
            }
        }

        /** 获取长度为len 的字符串 @param str @param num @param len @returns */ function padNum(str, num, len) {
            var temp = '';
            for (var i = 0; i < len; temp += num, i++) ;
            return temp = (temp += str).substr(temp.length - len);
        }
    }
}

/** 动态封装html */ function fEleA(text, flag, url, bgCor) {
    var style = ' style="font-weight: bold; color:#333;background-color:' + bgCor + ';" ';
    var cla = ' class="highlight"';
    var o = document.createElement('span');
    var str = '';
    if (flag == "") {
        o.innerHTML = text;
        return o;
    }
    var re = new RegExp('(' + flag + ')', 'gi');
    if (url) {
        str = text.replace(re, '<a href="' + url + '$1"' + style + '>$1</a>'); /*这里是给关键字加链接，红色的$1是指上面链接地址后的具体参数。*/
    } else {
        text = replaceAll(text, '&gt;', '>');
        str = text.replace(re, '<span ' + style + cla + '>$1</span>'); /*不加链接时显示*/
    }
    o.innerHTML = checkXssHTML(str);
    return o;
}

/** uem */ var common_uem = {refer: $("#referer").val(), domain: $('#domain').val(), lang: $('#lang').val()};

function getUemIdByToolType(toolType, type) {
    let uemIdMap = $("#uemIdMap").text();
    if (uemIdMap[0] == '{') {
        uemIdMap = JSON.parse(uemIdMap);
    } else {
        return "";
    }
    return uemIdMap[toolType][type];
}

function getUemDataByToolType(userRole,toolType, category, model, version, keyword, componentType) {
    let params = {
        'refer': common_uem.refer,
        'domain': common_uem.domain,
        'language': common_uem.lang,
        'userRole':userRole,
        'toolName': toolType,
        'category': category ? category : '',
        'model': model ? model : '',
        'version': version ? version : '',
        'keyword': keyword ? keyword : '',
        'componentType': componentType ? componentType : '',
        'pageType': commonUrl.pageType
    };
    return params;
}

function trackUemByDataAndId(params, uemId) {
    window.hwa('trackStructEvent', {uem_id: uemId, data: params});
}

function trackUemByDataAndPage(pageId, extend) {
    let params = {'refer': common_uem.refer, 'domain': common_uem.domain, 'language': common_uem.lang};
    if (!$.isEmptyObject(extend)) {
        $.extend(params, extend)
    }
    window.hwa('trackPageView', pageId, {data: params});
}

/** 意见反馈 */ $(function () {
    $.getJSON($("#common-chang-path").val() + "/pages/commonFile/commonShareLink.json", function (result) {
        urlJsonCommon = result;
        getShare();
        herfEmail();
        herfIn();
        hrefWeibo();
        getCollapse();
    })
});

/** 分享按钮 */ function getShare() {
    $('#feedDiv .share_btn').off('click').on('click', function (e) {
        stopBubble(e);
        const $share = $('#feedDiv .share');
        if ($share.is(':visible')) {
            $share.hide();
        } else {
            $share.show();
        }
    })
}

/** in分享 */ function herfIn() {
    $('.inHerf').on('click', function () {
        const newW = window.open(urlJsonCommon.shareLink.linkIn + encodeURIComponent(window.location.href));
        newW.opener = null;
    })
}

/** 邮件分享 */ function herfEmail() {
    $('.emailHerf').on('click', function () {
        if ($('#surveyModule').val() === 'encyclopedia-singleentity') {
            let pageTitle = $('#entityInfo_webPageTitle').val().replace('- Huawei', '').replace('- 华为', '').replace(/&/g, '%26');
            const abstract = $('.abstract p').text().replace(/<br[^>]*>/g, '%0d%0a').replace(/&/g, '%26');
            const newStr = strTrunc(abstract);
            const tip = $('#lang').val() === 'zh' ? '详情请访问：' : 'For more information,visit: ';
            const url = 'mailto:?subject=' + pageTitle + '&body=' + newStr + '%0d%0a%0d%0a' + tip + encodeURIComponent(window.location.href);
            const newW = window.open(url);
            newW.opener = null;
        } else {
            const pageTitle = $(document).attr('title').replace('- Huawei', '').replace('- 华为', '').replace(/&/g, '%26');
            const abstract = $(this).children().attr('value').replace(/<br[^>]*>/g, '%0d%0a').replace(/&/g, '%26');
            const tip = $('#lang').val() === 'zh' ? '详情请访问：' : 'For more information,visit: ';
            const url = 'mailto:?subject=' + pageTitle + '&body=' + abstract + '%0d%0a%0d%0a' + tip + encodeURIComponent(window.location.href);
            const newW = window.open(url);
            newW.opener = null;
        }
    })
}

function strTrunc(s) {
    var count = 0;
    for (var i = 0; i < s.length; i++) {
        count += encodeURIComponent(s.charAt(i)).length;
        if (count > 1400) return s.substring(0, i - 1) + "...";
    }
    ;
    return s;
};const hrefWeibo = () => {
    $('.share .sina').off('click').on('click', function (e) {
        stopBubble(e);
        const l = $(window).width() / 2 - 400, t = $(window).height() / 2 - 250;
        if ($('#surveyModule').val() === 'encyclopedia-singleentity') {
            let pageTitle = $('#entityInfo_webPageTitle').val().replace('- Huawei', '').replace('- 华为', '');
            let shareImage = $('#entityInfo_image').val().split('src="')[1].split('"')[0];
            const param = {
                url: window.location.href,
                type: '3',
                count: '1',
                /** 是否显示分享数，1显示(可选)*/ appkey: '',
                /** 您申请的应用appkey,显示分享来源(可选)*/ title: pageTitle,
                /** 分享的文字内容(可选，默认为所在页面的title)*/ pic: shareImage,
                /**分享图片的路径(可选)*/ ralateUid: '',
                /**关联用户的UID，分享微博会@该用户(可选)*/ rnd: new Date().valueOf(),
            };
            const temp = [];
            for (let p in param) {
                temp.push(p + '=' + encodeURIComponent(param[p] || ''));
            }
            const targetUrl = urlJsonCommon.shareLink.weibo + temp.join('&');
            const newH = window.open(targetUrl, 'sinaweibo', `width=800,height=500,left=${l},top= ${t}`);
            newH.opener = null;
        } else {
            const targetUrl = $(this).children().attr('value');
            const newH = window.open(targetUrl, 'sinaweibo', `width=800,height=500,left=${l},top= ${t}`);
            newH.opener = null;
        }
    })
};/** 意见反馈 *//*登录的相关属性*/
var queryParam = {language: null, loginPreURL: "", isLogin: null, requestURL: "", lang: $('#lang').val()};
$(function () {
    loadDialog();/*点击意见反馈按钮，详情页和切换页签的点击事件*/
    $("#feedback,#feedback2").off("click").on("click", function (event) {
        stopBubble(event);/* 没登陆跳转登陆界面*/
        if (JSON.parse($('#isLogin').val())) {/*设置该页面的pageId*/
            setPageId();/* 加载意见反馈弹出框的内容*/
            loadFeedBackIframe("#doc_feedback", "id_frm_docfeedback", "setCustomerFeedbackHeight");/*加载意见反馈弹窗*/
            loadFeedbackButton();
            $('#autoReply').siblings('.ui-dialog').css('top', '250px');
        } else {/* 登陆*/
            forLogin();
        }
    });
});

function getUserInfo() {
    var comPath = $('#common-chang-path').val();
    $.ajax({
        type: "post",
        url: comPath + "/user/getHeadInfo",
        dataType: "json",
        cache: false,
        async: false,
        success: function (data) {/*queryParam.language = HTMLDecode(data.language);*/
            queryParam.language = 'en';
            queryParam.loginPreURL = HTMLDecode(data.loginPreURL);
            queryParam.isLogin = data.isLogin;
        },
        error: function () {
        }
    });
}/*将字符串转HTML*/
function HTMLDecode(value) {
    try {
        value = value.replace(/</g, "&lt;");
        value = value.replace(/>/g, "&gt;");
        var temp = document.createElement("div");
        temp.innerHTML = value;
        var output = temp.innerText || temp.textContent;
        if (output == "undefined") {
            output = "";
        }
        temp = null;
        return output;
    } catch (e) {
        return value;
    }
}

function forLogin(keyword) {
    let newHref = checkXssUrl(location.href);
    if (keyword) {
        let keys = getHrefkeyword();
        if (keys) {
            newHref = newHref.replace(keys, keyword);
        }
    }
    const loginPreURL = checkXssUrl($('#loginPreURL').val());
    if (!loginPreURL) {
        return;
    }
    infofinderLogout();
    var domainStr = $('#domainStr').val();
    if (domainStr == 'carrier' && $('#thisPage').val() == 'infoFinder') {
        newHref = newHref.replace('.html', '');
        location.href = loginPreURL + encodeURIComponent(newHref.replace(/\(/g, '%28').replace(/\)/g, '%29'))
    } else {
        location.href = loginPreURL + encodeURIComponent(newHref.replace(/\(/g, '%28').replace(/\)/g, '%29'));
    }
}

/** 判断浏览器是PC端/移动端 @returns {boolean} true 移动端/false PC端 @since 2020年10月17日19点40分 */ function isMobile() {
    if (navigator.userAgent.match(/(phone|pod|iPhone|iPod|ios|Android|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i) || window.innerWidth <= 768) {
        return true;
    }
    return false;
}

/** 加载对话框 */ function loadDialog() {
    var windowWidth = window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth;
    var windowHeight = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
    var feedbackWidth = windowWidth;
    var headerDIVHeight = $("#headerDIV").height() + $('#headerTop').height();
    var feedbackHeight = windowHeight - 2 * headerDIVHeight;/* 解决页头把意见反馈dialog的头部挡住了*/
    if (!isMobile()) {
        feedbackWidth = windowWidth * 1000 / 1680;
        if (feedbackWidth > 1000) {
            feedbackWidth = 1000;
        }
        if (feedbackWidth < 850) {
            feedbackWidth = 850;
        }
        feedbackHeight = windowHeight * 540 / 950;
        if (feedbackHeight > 540) {
            feedbackHeight = 540;
        }
    }
    var feedbackTitle = "意见反馈";
    var lang = $("#lang").val();
    if ("zh" != lang) {
        feedbackTitle = "Feedback";
    }
    $("#doc_feedback").dialog({
        autoOpen: false,
        title: feedbackTitle,
        resizable: false,
        width: feedbackWidth,
        height: feedbackHeight,
        modal: true,
        zIndex: 200,
        close: function () {
            $("#doc_feedback").dialog("close");
        }
    });
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close").append("<span class='ui-button-icon ui-icon ui-icon-closethick'></span><span class='ui-button-icon-space'></span>");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("position", "absolute");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("top", "50%");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("left", "50%");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("margin-top", "-8px");
    $(".ui-dialog .ui-dialog-titlebar .ui-dialog-titlebar-close .ui-button-icon").css("margin-left", "-8px");
}

/** 意见反馈弹出显示 */ function loadFeedbackButton() {
    $("#dialog:ui-dialog").dialog("destroy");
    $("#doc_feedback").dialog("open");
    if (isMobile()) {
        $("#doc_feedback").css({"overflow": "hidden", "overflow-y": "auto", "max-height": "600px", "height": "auto"});
    }
    $("div[aria-describedby='doc_feedback'] .ui-dialog-title").css("text-align", "left");
}

function setCustomerFeedbackHeight(heights) {
    document.getElementById('id_frm_docfeedback').style.height = heights + 'px';
}

function getSafeLink(link) {
    if (link && link.indexOf('http') != 0 && link.indexOf('/') != 0) {
        return "javascript:void(0);";
    }
    return link;
}

function getFeedbackTopicId() {
    var pageId = $('#page_id').val();
    return pageId;
}/** 说明：该方法是意见反馈支持多页面的功能 data:2021-10-8 author: *//*得到页面的PageId*/
function initPageId(options, id) {
    if (!options && !(options instanceof Object)) {
        return false;
    }
    var pathName = $('#' + id).val();
    var URL = window.location.href.replace(window.location.origin, '');
    var lungs = $('#lang').val();
    var domainStr = $('#domain').val();
    var newLang;
    if (lungs != 'zh' && lungs != 'en') {
        newLang = 'en';
    } else {
        newLang = lungs;
    }
    var initParams = {
        lang: newLang,
        domain: domainStr,
        module: '',/*页面归属*/
        keyword: '',
        pbiId: 'NA',
        url: URL,
        callType: 'createIfNo',
    };
    $.extend(initParams, options);
    $.ajax({
        contentType: 'application/x-www-form-urlencoded; charset=UTF-8',
        type: 'post',
        url: pathName + '/pageId/queryOrGeneratedPageId',
        async: false,
        dataType: 'json',
        cache: false,
        data: initParams,
        success: function (res) {
            var datas = res.data;
            if (datas) {
                var pageId = datas.pageId;
                $('#page_id').val(pageId);
            }
        },
        error: function (err) {
        }
    })
}

function loadFeedBackIframe(iframeArea, iframeId, setHeightFun) {
    $(iframeArea).empty();/*在意见反馈的ifram添加遮罩避免子页面引起主页面的悬浮框无法定位*/
    var shield = $('<div class="shield" style="display: none; position: absolute; width: 100%; height: 100%;z-index: 799;"></div>');
    $(iframeArea).append(shield);
    var obj = $('<iframe id="' + iframeId + '"  scrolling="no" width="100%" height="600px" frameborder="0"></iframe>');
    $(iframeArea).append(obj);
    var srcLink = getIframeSrc();
    $("#" + iframeId).attr("src", getSafeLink(srcLink));
}

function getIframeSrc() {/*nodeId = "FB_TL1000000015",lang="zh",是否支持单页面：true*/
    var src = "";
    var langs = $('#lang').val();/*判断当前的环境*/
    var isTest = $('#isTest').val();
    if (isTest == "uat") {/*测试*/
        if (langs == 'zh') {
            src = location.origin + '/feedbackweb/feedback/feedbackFacade.jsp?params=%7B%22nodeId%22%3A%22FB_TL1000000015%22%2C%22modelId%22%3A%22SUP_OLTOOL%22%2C%22source%22%3A%228%22%2C%22heightCallback%22%3A%22setCustomerFeedbackHeight%22%2C%22loginCallback%22%3A%22login%22%2C%22canFeedback%22%3A%22true%22%2C%22refresh%22%3A%22N%22%2C%22nodeTypeName%22%3A%22info-Finder%22%2C%22lang%22%3A%22zh%22%2C%22subTypeId%22%3A%22%22%2C%22canReply%22%3A%22true%22%2C%22canHWEReplyAll%22%3A%22true%22%2C%22showTypeScoreandRate%22%3A%22false%22%2C%22showSolvedRate%22%3A%22true%22%2C%22showFeedbackScope%22%3A%222%22%2C%22callbackURL%22%3A%22%22%2C%22supportSinglePage%22%3A%22true%22%2C%22token%22%3A%22KMS0011X2021100609512240cfSUnBHNkB9VdFU45u%2FcLPJbLUPcM7AsWPPWj%2FSqhLQ0jYBtYXHaJ8nFBAqNWaeDvARBXqhX1hXZwT5jVgFzEVCuvHKcOXGK6gvaFYebeCIrXvg%3D%3D%22%7D';
        } else {
            src = location.origin + '/feedbackweb/feedback/feedbackFacade.jsp?params=%7B%22nodeId%22%3A%22FB_TL1000000015%22%2C%22modelId%22%3A%22SUP_OLTOOL%22%2C%22source%22%3A%228%22%2C%22heightCallback%22%3A%22setCustomerFeedbackHeight%22%2C%22loginCallback%22%3A%22login%22%2C%22canFeedback%22%3A%22true%22%2C%22refresh%22%3A%22N%22%2C%22nodeTypeName%22%3A%22info-Finder%22%2C%22lang%22%3A%22en%22%2C%22subTypeId%22%3A%22%22%2C%22canReply%22%3A%22true%22%2C%22canHWEReplyAll%22%3A%22true%22%2C%22showTypeScoreandRate%22%3A%22false%22%2C%22showSolvedRate%22%3A%22true%22%2C%22showFeedbackScope%22%3A%222%22%2C%22callbackURL%22%3A%22%22%2C%22supportSinglePage%22%3A%22true%22%2C%22token%22%3A%22KMS0011X2021100609512240cfSUnBHNkB9VdFU45u%2FcLPJbLUPcM7AsWPPWj%2FSqhLQ0jYBtYXHaJ8nFBAqNWaeDvARCQghR84tWnq4d4bP176tiaQbjQH7E8G9wOFnxg6NMEMQ%3D%3D%22%7D';
        }
    } else {/*生产*/
        if (langs == 'zh') {
            src = location.origin + '/feedbackweb/feedback/feedbackFacade.jsp?params=%7B%22nodeId%22%3A%22FB_TL1000000015%22%2C%22modelId%22%3A%22SUP_OLTOOL%22%2C%22source%22%3A%228%22%2C%22heightCallback%22%3A%22setCustomerFeedbackHeight%22%2C%22loginCallback%22%3A%22login%22%2C%22canFeedback%22%3A%22true%22%2C%22refresh%22%3A%22N%22%2C%22nodeTypeName%22%3A%22%E4%BF%A1%E6%81%AF%E9%80%9F%E6%9F%A5%22%2C%22lang%22%3A%22zh%22%2C%22subTypeId%22%3A%22%22%2C%22canReply%22%3A%22true%22%2C%22canHWEReplyAll%22%3A%22true%22%2C%22showTypeScoreandRate%22%3A%22false%22%2C%22showSolvedRate%22%3A%22true%22%2C%22showFeedbackScope%22%3A%222%22%2C%22callbackURL%22%3A%22%22%2C%22supportSinglePage%22%3A%22true%22%2C%22token%22%3A%22KMS0011X2021100715205240lFZD7Z3NHWmBmTcfqRRmkYyKseKXXO60AYisW%2FRB0aerq%2FkPGoSowHoa4F8BXRi8vARAwd2WNGvsALFp82lI0lWtkhyMMUHr3fYV9G8i1%2BOZMQQ%3D%3D%22%7D&lang=zh';
        } else {
            src = location.origin + '/feedbackweb/feedback/feedbackFacade.jsp?params=%7B%22nodeId%22%3A%22FB_TL1000000015%22%2C%22modelId%22%3A%22SUP_OLTOOL%22%2C%22source%22%3A%228%22%2C%22heightCallback%22%3A%22setCustomerFeedbackHeight%22%2C%22loginCallback%22%3A%22login%22%2C%22canFeedback%22%3A%22true%22%2C%22refresh%22%3A%22N%22%2C%22nodeTypeName%22%3A%22Search%2BCenter%22%2C%22lang%22%3A%22en%22%2C%22subTypeId%22%3A%22%22%2C%22canReply%22%3A%22true%22%2C%22canHWEReplyAll%22%3A%22true%22%2C%22showTypeScoreandRate%22%3A%22false%22%2C%22showSolvedRate%22%3A%22true%22%2C%22showFeedbackScope%22%3A%222%22%2C%22callbackURL%22%3A%22%22%2C%22supportSinglePage%22%3A%22true%22%2C%22token%22%3A%22KMS0011X2021100715205240lFZD7Z3NHWmBmTcfqRRmkYyKseKXXO60AYisW%2FRB0aerq%2FkPGoSowHoa4F8BXRi8vARAqbUEavxQ6zyKBooNa3wXde8aHuHa5sTRt5LUEErm5NQ%3D%3D%22%7D&lang=en';
        }
    }
    return src;
}/*信息速查的页面ID*/
function getInfoFinderPageId() {
    var whichPage = $('#whichPage').val();
    if (whichPage == "search-center-index") {
        var name = $('.info-nav-content').find('.info-nav-cur a').attr('value');
        var option = {};
        switch (name) {
            case "switch":
                option = {module: 'search-center', keyword: 'search-center switch'};
                break;
            case "routers":
                option = {module: 'search-center', keyword: 'search-center routers'};
                break;
            case "security":
                option = {module: 'search-center', keyword: 'search-center security'};
                break;
            case "wlan":
                option = {module: 'search-center', keyword: 'search-center wlan'};
                break;
            case "transmission-network":
                option = {module: 'search-center', keyword: 'search-center transmission-network'};
                break;
            case "access-network":
                option = {module: 'search-center', keyword: 'search-center access-network'};
                break;
            case "network-management-and-analysis-software":
                option = {module: 'search-center', keyword: 'search-center network-management-and-analysis-software'};
                break;
            case "fm-switch":
                option = {module: 'search-center', keyword: 'search-center fm-switch'};
                break;
            case "solution":
                option = {module: 'search-center', keyword: 'search-center solution'};
                break;
            default:
                option = {module: 'search-center', keyword: 'search-center index'};
                break;
        }
        initPageId(option, 'common-chang-path');
    } else if (whichPage == "search-center-overView") {
        var overOption = null;
        var pid = $('#pid').val();
        var urlKey = $('.info_cat-left').find('.cistern .info-cat-cur').attr('urlKey');
        switch (urlKey) {
            case "":
                overOption = {module: 'search-center', keyword: 'product overview', pbiId: pid};
                break;
            case "product-image-gallery":
                overOption = {module: 'search-center', keyword: 'image', pbiId: pid};
                break;
            case "eom-eofs-and-eos-date":
                overOption = {module: 'search-center', keyword: 'eox', pbiId: pid};
                break;
            case "specifications":
                overOption = {module: 'search-center', keyword: 'specification', pbiId: pid};
                break;
            case "hardware-configuration-tool":
                overOption = {module: 'search-center', keyword: 'hardware configuration', pbiId: pid};
                break;
            case "web3d":
                overOption = {module: 'search-center', keyword: '3d', pbiId: pid};
                break;
            case "hardwarecenter":
                overOption = {module: 'search-center', keyword: 'hardware center', pbiId: pid};
                break;
            case "speccenter":
                overOption = {module: 'search-center', keyword: 'feature query', pbiId: pid};
                break;
            case "license-control-items":
                overOption = {module: 'search-center', keyword: 'license', pbiId: pid};
                break;
            case "alarms":
                overOption = {module: 'search-center', keyword: 'alarm', pbiId: pid};
                break;
            case "commands":
                overOption = {module: 'search-center', keyword: 'command', pbiId: pid};
                break;
            case "logs":
                overOption = {module: 'search-center', keyword: 'log', pbiId: pid};
                break;
            case "mib":
                overOption = {module: 'search-center', keyword: 'mib', pbiId: pid};
                break;
            default:
                break;
        }
        initPageId(overOption, 'common-chang-path');
    } else if (whichPage == "search-center-overView-new") {
        var overOption = null;
        var pid = $('#pid').val();
        var urlKey = $('.info_cat-left').find('.cistern .info-cat-cur').attr('urlKey');
        if (undefined == urlKey) {
            urlKey = "";
        }
        switch (urlKey) {
            case "":
                overOption = {module: 'search-center', keyword: 'product overview', pbiId: pid};
                break;
            default:
                break;
        }
        initPageId(overOption, 'common-chang-path');
    }
}/*得到百科的PageId*/
function getEncyclopediaPageId() {
    var keyW = $('#entityInfo_name').val();
    var option = {module: 'encyclopedia',/*页面归属*/ keyword: keyW,};
    initPageId(option, 'contextPath');
}/*得到开放式搜索首页的PageId*/
function getOpenSerachPageId() {
    var whichPage = $('#whichPage').val();
    var option = null;
    switch (whichPage) {
        case "infofinder-index":
            option = {module: 'info-finder',/*页面归属*/ keyword: 'info-finder index'};
            break;
        case "infofinder-result":
            option = {module: 'info-finder',/*页面归属*/ keyword: 'info-finder seach result'};
            break;
        default:
            break;
    }
    initPageId(option, 'common-chang-path');
}/*得到选型的PageId*/
function getModelSelectorPageId() {
    let whichSubPage = $('#whichSubPage').val();
    let option = null;
    switch (whichSubPage) {
        case "model-selector index":
            option = {module: 'model-selector',/*页面归属*/ keyword: 'model-selector index'};
            break;
        case "model-selector doc":
            option = {module: 'model-selector',/*页面归属*/ keyword: 'model-selector doc'};
            break;
        case "model-selector auto":
            option = {module: 'model-selector',/*页面归属*/ keyword: 'model-selector auto'};
            break;
        case "model-selector quick":
            option = {module: 'model-selector',/*页面归属*/ keyword: 'model-selector quick'};
            break;
        default:
            break;
    }
    initPageId(option, 'common-chang-path');
}/*得到独立工具的PageId*/
function getUXToolsPageId(type) {
    let topicType = $('#topicType').val();
    let option = null;
    if (topicType) {
        option = {module: type,/*页面归属*/ keyword: topicType}
    }
    initPageId(option, 'common-chang-path');
}/*得到解决方案书架的PageId*/
function getSolutionPageId() {
    var pduName = $('#pdu_name').val();
    var modal = $('#modal').val();
    if (modal == 'industry') {
        pduName = modal + '-' + pduName
    }
    let option = null;
    if (pduName) {
        option = {module: 'solutionDocument',/*页面归属*/ keyword: pduName}
    }
    initPageId(option, 'common-chang-path');
}/*得到运营商书架的PageId*/
function getDocumentShelfPageId() {
    var thisTitle = $('#thisTitle').val();
    var pduName = $('#pdu_name').val();
    if (pduName == 'IPE') {
        pduName = 'IPv6+';
    }
    let option = null;
    if (pduName) {
        option = {module: thisTitle + 'Document',/*页面归属*/ keyword: pduName};
    }
    initPageId(option, 'common-chang-path');
}/*根据不同的工具来获取和设置页面ID*/
function setPageId() {
    var whichPage = $('#whichPage').val();
    switch (whichPage) {
        case "infofinder-index":
            getOpenSerachPageId();
            break;
        case "infofinder-result":
            getOpenSerachPageId();
            break;
        case "search-center-index":
            getInfoFinderPageId();
            break;
        case "search-center-overView":
            getInfoFinderPageId();
            break;
        case "encyclopedia":
            getEncyclopediaPageId();
            break;
        case "model-selector":
            getModelSelectorPageId();
            break;
        case "infofinder-tools":
            getUXToolsPageId("tool");
            break;
        case "infofinder-solution":
            getSolutionPageId();
            break;
        case "infofinder-document":
            getDocumentShelfPageId();
            break;
        case "search-center-overView-new":
            getInfoFinderPageId();
            break;
        case "search-center-overViewUX":
            getUXToolsPageId("search-center");
            break;
        default:
            break;
    }
}

/**底部*/ !function (g, f, j, i) {
    var h = g(f);
    g.fn.lazyload = function (e) {
        function d() {
            var k = 0;
            b.each(function () {
                var l = g(this);
                if (!a.skip_invisible || l.is(":visible")) {
                    if (g.abovethetop(this, a) || g.leftofbegin(this, a)) {
                    } else {
                        if (g.belowthefold(this, a) || g.rightoffold(this, a)) {
                            if (++k > a.failure_limit) {
                                return !1;
                            }
                        } else {
                            l.trigger("appear"), k = 0;
                        }
                    }
                }
            })
        }

        var c, b = this, a = {
            threshold: 0,
            failure_limit: 0,
            event: "scroll",
            effect: "show",
            container: f,
            data_attribute: "original",
            skip_invisible: !1,
            appear: null,
            load: null,
            placeholder: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAANSURBVBhXYzh8+PB/AAffA0nNPuCLAAAAAElFTkSuQmCC"
        };
        return e && (i !== e.failurelimit && (e.failure_limit = e.failurelimit, delete e.failurelimit), i !== e.effectspeed && (e.effect_speed = e.effectspeed, delete e.effectspeed), g.extend(a, e)), c = a.container === i || a.container === f ? h : g(a.container), 0 === a.event.indexOf("scroll") && c.on(a.event, function () {
            return d();
        }), this.each(function () {
            var k = this, l = g(k);
            k.loaded = !1, (l.attr("src") === i || l.attr("src") === !1) && l.is("img") && l.attr("src", a.placeholder), l.one("appear", function () {
                if (!this.loaded) {
                    if (a.appear) {
                        var m = b.length;
                        a.appear.call(k, m, a);
                    }
                    g("<img />").on("load", function () {
                        var p = l.attr("data-" + a.data_attribute);
                        l.hide(), l.is("img") ? l.attr("src", p) : l.css("background-image", "url('" + p + "')"), l[a.effect](a.effect_speed), k.loaded = !0;
                        var o = g.grep(b, function (q) {
                            return !q.loaded;
                        });
                        if (b = g(o), a.load) {
                            var n = b.length;
                            a.load.call(k, n, a);
                        }
                    }).attr("src", l.attr("data-" + a.data_attribute));
                }
            }), 0 !== a.event.indexOf("scroll") && l.on(a.event, function () {
                k.loaded || l.trigger("appear")
            })
        }), h.on("resize", function () {
            d()
        }), /(?:iphone|ipod|ipad).*os 5/gi.test(navigator.appVersion) && h.on("pageshow", function (k) {
            k.originalEvent && k.originalEvent.persisted && b.each(function () {
                g(this).trigger("appear");
            })
        }), g(j).ready(function () {
            d();
        }), this
    } , g.belowthefold = function (d, b) {
        var a;
        return a = b.container === i || b.container === f ? (f.innerHeight ? f.innerHeight : h.height()) + h.scrollTop() : g(b.container).offset().top + g(b.container).height(), a <= g(d).offset().top - b.threshold
    } , g.rightoffold = function (d, b) {
        var a;
        return a = b.container === i || b.container === f ? h.width() + h.scrollLeft() : g(b.container).offset().left + g(b.container).width(), a <= g(d).offset().left - b.threshold
    } , g.abovethetop = function (d, b) {
        var a;
        return a = b.container === i || b.container === f ? h.scrollTop() : g(b.container).offset().top, a >= g(d).offset().top + b.threshold + g(d).height()
    } , g.leftofbegin = function (d, b) {
        var a;
        return a = b.container === i || b.container === f ? h.scrollLeft() : g(b.container).offset().left, a >= g(d).offset().left + b.threshold + g(d).width()
    } , g.inviewport = function (a, d) {
        return !(g.rightoffold(a, d) || g.leftofbegin(a, d) || g.belowthefold(a, d) || g.abovethetop(a, d))
    } , g.extend(g.expr[":"], {
        "below-the-fold": function (a) {
            return g.belowthefold(a, {threshold: 0})
        }, "above-the-top": function (a) {
            return !g.belowthefold(a, {threshold: 0})
        }, "right-of-screen": function (a) {
            return g.rightoffold(a, {threshold: 0})
        }, "left-of-screen": function (a) {
            return !g.rightoffold(a, {threshold: 0})
        }, "in-viewport": function (a) {
            return g.inviewport(a, {threshold: 0})
        }, "above-the-fold": function (a) {
            return !g.belowthefold(a, {threshold: 0})
        }, "right-of-fold": function (a) {
            return g.rightoffold(a, {threshold: 0})
        }, "left-of-fold": function (a) {
            return !g.rightoffold(a, {threshold: 0})
        }
    })
}(jQuery, window, document);
!function (a) {
    a.fn.hoverIntent = function (q, e, d) {
        var b = {interval: 100, sensitivity: 6, timeout: 0};
        b = "object" == typeof q ? a.extend(b, q) : a.isFunction(e) ? a.extend(b, {
            over: q,
            out: e,
            selector: d
        }) : a.extend(b, {over: q, out: q, selector: e});
        var m, f, p, w, g = function (c) {
            m = c.pageX, f = c.pageY
        }, l = function (c, h) {
            return h.hoverIntent_t = clearTimeout(h.hoverIntent_t), Math.sqrt((p - m) * (p - m) + (w - f) * (w - f)) < b.sensitivity ? (a(h).off("mousemove.hoverIntent", g), h.hoverIntent_s = !0, b.over.apply(h, [c])) : (p = m, w = f, h.hoverIntent_t = setTimeout(function () {
                l(c, h)
            }, b.interval), void 0)
        }, k = function (h, c) {
            return c.hoverIntent_t = clearTimeout(c.hoverIntent_t), c.hoverIntent_s = !1, b.out.apply(c, [h])
        }, j = function (c) {
            var i = a.extend({}, c), h = this;
            h.hoverIntent_t && (h.hoverIntent_t = clearTimeout(h.hoverIntent_t)), "mouseenter" === c.type ? (p = i.pageX, w = i.pageY, a(h).on("mousemove.hoverIntent", g), h.hoverIntent_s || (h.hoverIntent_t = setTimeout(function () {
                l(i, h)
            }, b.interval))) : (a(h).off("mousemove.hoverIntent", g), h.hoverIntent_s && (h.hoverIntent_t = setTimeout(function () {
                k(i, h)
            }, b.timeout)))
        };
        return this.on({"mouseenter.hoverIntent": j, "mouseleave.hoverIntent": j}, b.selector)
    }
}(jQuery);
+function (b) {
    function a() {
        var e = document.createElement("bootstrap");
        var d = {
            WebkitTransition: "webkitTransitionEnd",
            MozTransition: "transitionend",
            OTransition: "oTransitionEnd otransitionend",
            transition: "transitionend"
        };
        for (var c in d) {
            if (e.style[c] !== undefined) {
                return {end: d[c]};
            }
        }
        return false;
    }

    b.fn.emulateTransitionEnd = function (e) {
        var d = false;
        var c = this;
        b(this).one("bsTransitionEnd", function () {
            d = true;
        });
        var f = function () {
            if (!d) {
                b(c).trigger(b.support.transition.end);
            }
        };
        setTimeout(f, e);
        return this;
    };
    b(function () {
        b.support.transition = a();
        if (!b.support.transition) {
            return;
        }
        b.event.special.bsTransitionEnd = {
            bindType: b.support.transition.end,
            delegateType: b.support.transition.end,
            handle: function (c) {
                if (b(c.target).is(this)) {
                    return c.handleObj.handler.apply(this, arguments);
                }
            }
        }
    })
}(jQuery);
+function (d) {
    var e = function (g, f) {
        this.$element = d(g);
        this.options = d.extend({}, e.DEFAULTS, f);
        this.$trigger = d('[data-toggle="collapse"][href="#' + g.id + '"],[data-toggle="collapse"][data-target="#' + g.id + '"]');
        this.transitioning = null;
        if (this.options.parent) {
            this.$parent = this.getParent();
        } else {
            this.addAriaAndCollapsedClass(this.$element, this.$trigger);
        }
        if (this.options.toggle) {
            this.toggle();
        }
    };
    e.VERSION = "3.3.7";
    e.TRANSITION_DURATION = 350;
    e.DEFAULTS = {toggle: true};
    e.prototype.dimension = function () {
        var f = this.$element.hasClass("width");
        return f ? "width" : "height"
    };
    e.prototype.show = function () {
        if (this.transitioning || this.$element.hasClass("in")) {
            return;
        }
        var h;
        var j = this.$parent && this.$parent.children(".panel").children(".in, .collapsing");
        if (j && j.length) {
            h = j.data("bs.collapse");
            if (h && h.transitioning) {
                return;
            }
        }
        var g = d.Event("show.bs.collapse");
        this.$element.trigger(g);
        if (g.isDefaultPrevented()) {
            return;
        }
        if (j && j.length) {
            b.call(j, "hide");
            h || j.data("bs.collapse", null);
        }
        var k = this.dimension();
        this.$element.removeClass("collapse").addClass("collapsing")[k](0).attr("aria-expanded", true);
        this.$trigger.removeClass("collapsed").attr("aria-expanded", true);
        this.transitioning = 1;
        var f = function () {
            this.$element.removeClass("collapsing").addClass("collapse in")[k]("");
            this.transitioning = 0;
            this.$element.trigger("shown.bs.collapse");
        };
        if (!d.support.transition) {
            return f.call(this);
        }
        var i = d.camelCase(["scroll", k].join("-"));
        this.$element.one("bsTransitionEnd", d.proxy(f, this)).emulateTransitionEnd(e.TRANSITION_DURATION)[k](this.$element[0][i]);
    };
    e.prototype.hide = function () {
        if (this.transitioning || !this.$element.hasClass("in")) {
            return;
        }
        var g = d.Event("hide.bs.collapse");
        this.$element.trigger(g);
        if (g.isDefaultPrevented()) {
            return;
        }
        var h = this.dimension();
        this.$element[h](this.$element[h]())[0].offsetHeight;
        this.$element.addClass("collapsing").removeClass("collapse in").attr("aria-expanded", false);
        this.$trigger.addClass("collapsed").attr("aria-expanded", false);
        this.transitioning = 1;
        var f = function () {
            this.transitioning = 0;
            this.$element.removeClass("collapsing").addClass("collapse").trigger("hidden.bs.collapse")
        };
        if (!d.support.transition) {
            return f.call(this);
        }
        this.$element[h](0).one("bsTransitionEnd", d.proxy(f, this)).emulateTransitionEnd(e.TRANSITION_DURATION)
    };
    e.prototype.toggle = function () {
        this[this.$element.hasClass("in") ? "hide" : "show"]();
    };
    e.prototype.getParent = function () {
        return d(this.options.parent).find('[data-toggle="collapse"][data-parent="' + this.options.parent + '"]').each(d.proxy(function (h, g) {
            var f = d(g);
            this.addAriaAndCollapsedClass(c(f), f)
        }, this)).end();
    };
    e.prototype.addAriaAndCollapsedClass = function (g, f) {
        var h = g.hasClass("in");
        g.attr("aria-expanded", h);
        f.toggleClass("collapsed", !h).attr("aria-expanded", h);
    };

    function c(f) {
        var g;
        var h = f.attr("data-target") || (g = f.attr("href")) && g.replace(/.*(?=#[^\s]+$)/, "");
        return d(h);
    }

    function b(f) {
        return this.each(function () {
            var i = d(this);
            var h = i.data("bs.collapse");
            var g = d.extend({}, e.DEFAULTS, i.data(), typeof f == "object" && f);
            if (!h && g.toggle && /show|hide/.test(JSON.parse(JSON.stringify(f)))) {
                g.toggle = false;
            }
            if (!h) {
                i.data("bs.collapse", (h = new e(this, g)));
            }
            if (typeof f == "string") {
                h[f]();
            }
        })
    }

    var a = d.fn.collapse;
    d.fn.collapse = b;
    d.fn.collapse.Constructor = e;
    d.fn.collapse.noConflict = function () {
        d.fn.collapse = a;
        return this;
    };
    d(document).on("click.bs.collapse.data-api", '[data-toggle="collapse"]', function (j) {
        var i = d(this);
        if (!i.attr("data-target")) {
            j.preventDefault();
        }
        var f = c(i);
        var h = f.data("bs.collapse");
        var g = h ? "toggle" : i.data();
        b.call(f, g);
    })
}(jQuery);

/** iframe 跨页面传参：发送消息 @param msg 必须是字符串，若为对象，需 stringify @param type 1：父=>子、2：子=>父 @param iframeEle 当type为1时需要，获取方式： document.getElementById("iframe_id") */ function sendMessage(msg, type, iframeEle, target) {
    if (type === 1) {/* 父 => 子*/
        iframeEle.contentWindow.postMessage(msg, target);
    } else if (type === 2) {/* 子 => 父*/
        window.parent.postMessage(msg, target);
    }
}

/** 字符串全局替换 @param src,oldStr,newStr 需要处理的字符串,需要替换部分,替换结果 @returns {string} 处理过的字符串 */ function replaceAll(src, oldStr, newStr) {
    return src.replace(new RegExp(oldStr, 'ig'), newStr);
}

/** 标签转换为实体符号 @param str 源字符串 @returns {string} 新字符串 */ function checkXssHTML(str) {
    str += "";
    str = str.replace(/&amp;/g, "&").replace(/&quot;/g, '"').replace(/&apos;/g, "'").replace(/&#x27;/g, "'").replace(/&#x2F;/g, "/").replace(/&#40;/g, "(").replace(/&#41;/g, ")");
    if (str) {
        const illeagalVal = ["alert(", "eval(", "prompt(", "script:", "window.", "location.href", "location.hash", "location.search", "confirm("];
        for (let i = 0; i < illeagalVal.length; i++) {
            if (str.indexOf(illeagalVal[i]) > -1 && str.indexOf('javascript:void(0)') == -1) {
                str = "";
                break;
            }
        }
    }
    return str;
}

/** 内容不能为url防止xss攻击，利用url外发 */ function checkXssInputUrl(str) {
    let newStr = (str + "").toLocaleLowerCase();
    if (newStr.indexOf('http://') > -1 || newStr.indexOf('https://') > -1 || newStr.indexOf('.com') > -1) {
        return true;
    }
    return false;
}

/** 正则特殊符号转义 */ function regReplaceHtml(str) {
    return str.replace(/\+/g, '\\+').replace(/\*/g, '\\*').replace(/\(/g, '\\(').replace(/\)/g, '\\)').replace(/\?/g, '\\?').replace(/\^/g, '\\^').replace(/\$/g, '\\$');
}

/** 判断智能问卷的投放 */ let survey = {
    viewModel: '',
    module: '',
    statusData: [],
    isFirst: true,
    target: location.host.indexOf('localhost') > -1 ? location.protocol + '//localhost.huawei.com:8083' : location.protocol + '//' + location.host,
};/*isReset 是否重置*/
getMessage(survey.target);
// 邮件订阅页面
/** 判断邮件订阅的弹出 */ let subscribe = {
    viewModel: '',
    module: '',
    statusData: [],
    isFirst: true,
    isPop:false,
    target: location.host.indexOf('localhost') > -1 ? location.protocol + '//localhost.huawei.com:8083' : location.protocol + '//' + location.host,
};/*isReset 是否重置*/
function getQuerySubscribe(isIgnoreSubscribe){
    if (isIgnoreSubscribe === 'N') {
        return false;
    } else if(isIgnoreSubscribe === 'Y') {
        return true
    }

};
//  判断当前订阅状态
 function querySubscribeStrategy(isAsync){
    let datas = {}
    $.ajax({
        contentType: 'application/json;charset=utf-8',
        type: "Post",
        url: $('#common-chang-path').val() + '/subscribe/querySubscribeStrategy',
        dataType: "json",
        async: isAsync ? true : false,
        cache: false,
        headers: {},
        data: JSON.stringify({lang: $('#lang').val(), domain: $('#domain').val()}),
        success: function (res) {
            var data = res.data;
            if (!data || data.length == 0) {
                datas = {};
            }
            datas = data;
            if (typeof callback == 'function') {
                i18n_detail.statusData = data;
                callback()
            }
        },
        error: function (err) {
        }
    })
    return datas
}
async function getSubscribe(module,active,userLevel){
    if (isMobile()) {/*暂时移动端不支持*/
        return;
    }
    const isLogin = $('#isLogin').val() === 'true' ? true : false;
    // 调用点击事件方法
    SubscribeClick(module)
    // 调注册表判断是否弹出
    let isPop = getQuerySubscribe(active)
    if(isPop){
        // 首次调用主动弹窗
        // 未登录时勾选弹出
            if(localStorage.getItem('isSubscribePop') == 'true'){
                loadSubscribeIframe(module, isPop, false);
                localStorage.setItem('isSubscribePop',false)
                return
            }
        // 判断权限是否是5，6
        if(isLogin) {
            if (userLevel == 5 || userLevel == 6) {
                subscribe.isPop = false
                if ($('#surveyModule').val() && $('#surveyModule').val() != 'speccenter') {/*规格查询特殊处理*/
                    getSurvey($('#surveyModule').val());
                }
                return;
            }
        }
        // 判断cookie
        if(getInfoCookie('Infofinder_ignore_subscribe_status') == '0'){
            subscribe.isPop = false
            if ($('#surveyModule').val() && $('#surveyModule').val() != 'speccenter') {/*规格查询特殊处理*/
                getSurvey($('#surveyModule').val());
            }
            return
        }
        if(isLogin){
            // 登录状态下根据接口设置cookie
            const data = await querySubscribeStrategy(false)
            if(data.status == 1 || data.status == 2){
                if(getInfoCookie('Infofinder_ignore_subscribe_status') != '0')
                setInfoCookie('Infofinder_ignore_subscribe_status', 0, 7,`/info-finder`);
                subscribe.isPop = false
                if ($('#surveyModule').val() && $('#surveyModule').val() != 'speccenter') {/*规格查询特殊处理*/
                    getSurvey($('#surveyModule').val());
                }
                return
            }
        }
            loadSubscribeIframe(module, isPop, false);
        }else{
        subscribe.isPop = false
        if ($('#surveyModule').val() && $('#surveyModule').val() != 'speccenter') {/*规格查询特殊处理*/
            getSurvey($('#surveyModule').val());
        }
        return
    }
}
// 初始化邮件订阅点击事件
function SubscribeClick(module) {
    $('#feedDiv').find('.subscribe').off('click').on('click', function (e) {
        stopBubble(e);

        if($('#survey_feedback').is(':visible')){
            survey.isFirst = false
            $('#survey_feedback').fadeOut(200);
        }

        if ($('#email_subscribe').is(':visible')) {
            return;
        }
        $('.header-right .lang ul').hide();
        loadSubscribeIframe(module, '', true);
    })
}
let autoTimer;
// 初始化弹窗
function loadSubscribeIframe(module,isPop,isClick){
    const isLogin = $('#isLogin').val() === 'true' ? true : false;
    // $('#email_subscribe').attr('style', ''); /*初次点击，移出所有修改样式，重置状态*/
    let params = {
        type: isClick ? 'firstClick' : 'load',
        domain: $('#domain').val(),
        domainStr: $('#domainStr').val(),
        lang: $('#lang').val(),
        module: module,
        url: location.href,
        isLogin: isLogin,
        isShow:false,
    };
    if (subscribe.isFirst) {
        $('#email_subscribe').attr('style', ''); /*初次点击，移出所有修改样式，重置状态*/
        // if (isClick) {/*未自动弹窗，初次点击，直接显示邮件*/
            let data = {status:null}
            if(isLogin){
                data = querySubscribeStrategy(false)
            }
            params.status = data.status
        // }
        subscribe.isPop = true
        if(!isClick){
            // 让表出现但是问卷不弹出
            if ($('#surveyModule').val() && $('#surveyModule').val() != 'speccenter') {/*规格查询特殊处理*/
                getSurvey($('#surveyModule').val());
            }
        }
            // 主动弹出
            const host = location.host;
            $('#email_subscribe').empty();
            let iframeSrc = host.indexOf('localhost') > -1 ? 'http://localhost.huawei.com:8083/info-finder/subscribeTip/'+ $('#lang').val()+ '/'+ $('#domain').val()+'/index' : location.protocol + location.port + '//' + host + '/info-finder/vue/subscribeTip/' + params.lang + '/' + params.domainStr + '/index';
            $('#email_subscribe').append(checkXssHTML('<iframe id="subscribe_iframe" width="420" height="232" scrolling="no" frameborder="0" src="' + iframeSrc + '"></iframe>'));
            let iframeEle = document.getElementById('subscribe_iframe')
            iframeEle.onload = function (e) {
                t0 = setTimeout(function () {
                    sendMessage(params, 1, iframeEle, subscribe.target);/*正常展示*/
                    subscribe.isFirst = false;
                    // setTimeout(function(){
                    //     iframeEle.style = 'margin: 15px;box-shadow: -2px -2px 10px 5px #7d7d7d1a;'
                    // },100)
                    if (!isPop) {
                        $('#email_subscribe').fadeIn(0);
                    }
                }, 1000);
                if (isPop) {
                    t1 = setTimeout(function () {
                        $('#email_subscribe').fadeIn(0);
                    },0);
                }
                setTimeout(()=>{
                    // 获取iframe的内容文档
                    var iframeDoc = iframeEle.contentDocument || iframeEle.contentWindow.document;
                    let email_subscribe =  iframeDoc.getElementsByClassName('subscribe');
                    if(!email_subscribe[0].textContent){
                        $('#email_subscribe').attr('style', 'display:none');
                    }
                },5000)
            }
    }else{
        // 点击弹出
            let iframeEle = document.getElementById('subscribe_iframe');
            $('#email_subscribe').attr('style', ''); /*初次点击，移出所以修改样式，重置状态*/
            // iframeEle.style = ''
            clearTimeout(t0);
            clearTimeout(t1);
            // 执行一次接口判断当前状态
            let data = {status:null}
            if(isLogin){
                data = querySubscribeStrategy(false)
            }
            params.status = data.status
            sendMessage(params, 1, iframeEle, subscribe.target);/*正常展示*/
            $('#email_subscribe').fadeIn(200);
        setTimeout(()=>{
            // 获取iframe的内容文档
            var iframeDoc = iframeEle.contentDocument || iframeEle.contentWindow.document;
            let email_subscribe =  iframeDoc.getElementsByClassName('subscribe');
            if(!email_subscribe[0].textContent){
                $('#email_subscribe').attr('style', 'display:none');
            }
        },5000)
        }
}

function getSurvey(module) {
    if (isMobile()) {/*暂时移动端不支持*/
        return;
    }
    let isPop = getQuerySurveyTime(module);
    SurveyClick(module);
    survey.module = module;
    // 邮件弹，问卷不弹，但是显示图标
    if(subscribe.isPop){
        return
    }
    const thisModule = module.indexOf('speccenter') > -1 ? 'speccenter' : module;
    if (isPop !== 'false' && !getInfoCookie('IgnoreSurvey_' + thisModule)) {
        loadSurveyIframe(module, isPop, false);
        return true
    }
}

function SurveyClick(module) {
    $('#feedDiv').find('.surVey').off('click').on('click', function (e) {
        stopBubble(e);
        if($('#email_subscribe').is(':visible')){
            $('#email_subscribe').fadeOut(200)
        }
        if ($('#survey_feedback').is(':visible')) {
            return;
        }
        $('.header-right .lang ul').hide();
        loadSurveyIframe(module, '', true);
    })
}

let timers, timers1 , t0 , t1;
function loadSurveyIframe(module, isPop, isClick) {
    const isLogin = $('#isLogin').val() === 'true' ? true : false;
    if (survey.isFirst) {
        if (isClick) {/*未自动弹窗，初次点击，直接显示问卷*/
            $('#survey_feedback').attr('style', ''); /*初次点击，移出所以修改样式，重置状态*/
            $('#survey_feedback').addClass(isLogin ? 'detail' : 'detailNo').removeClass('score');
        } else {/*自动弹窗，显示评分页*/
            $('#survey_feedback').removeClass(isLogin ? 'detail' : 'detailNo').addClass('score');
            if ($("#surveyModule").val() === 'encyclopedia-portal') {
                $('#survey_feedback').addClass('baike_short');
            } else {
                $('#survey_feedback').removeClass('baike_short');
            }
        }
        let params = {
            type: isClick ? 'firstClick' : 'load',
            domain: $('#domain').val(),
            domainStr: $('#domainStr').val(),
            lang: $('#lang').val(),
            module: module,
            url: location.href,
            isLogin: isLogin,
            toolId: 'FB_TL1000000015',
        };
        const host = location.host;
        $('#survey_feedback').empty();
        let iframeSrc = host.indexOf('localhost') > -1 ? location.protocol + '//localhost.huawei.com:8083/intelligent-survey' : location.protocol + location.port + '//' + host + '/info-finder/vue/intelligent-survey';
        $('#survey_feedback').append(checkXssHTML('<iframe id="survey_iframe" width="100%" height="100%" scrolling="no" frameborder="0" src="' + iframeSrc + '"></iframe>'));
        let iframeEle = document.getElementById('survey_iframe');
        iframeEle.onload = function (e) {
            timers = setTimeout(function () {
                const data = survey.statusData;
                if (data.isEnabled == 'Y') {
                    if (data.isRepeatedAllowed == 'N' && data.status.indexOf(2) > -1) {/*isRepeatedAllowed 为N，且status不包含 2 或者 isRepeatedAllowed 为Y*/
                        sendMessage({type: 'hasPopped', viewModel: survey.viewModel}, 1, iframeEle, survey.target);/*用户已作答过*/
                    } else {
                        sendMessage(params, 1, iframeEle, survey.target);/*正常展示*/
                    }
                }
                survey.isFirst = false;
                if (!isPop) {
                    $('#survey_feedback').fadeIn(200);
                }
            }, 300);
            if (isPop) {
                const time = isPop * 1000;
                timers1 = setTimeout(function () {
                    $('#survey_feedback').fadeIn(200);
                }, time);
            }


            setTimeout(()=>{
                // 获取iframe的内容文档
                var iframeDoc = iframeEle.contentDocument || iframeEle.contentWindow.document;
                let survey_page_content =  iframeDoc.getElementsByClassName('survey_page');
                if(!survey_page_content[0].textContent){
                    $('#survey_feedback').attr('style', 'display:none');
                }
            },5000)
        }
    } else {
        let iframeEle = document.getElementById('survey_iframe');
        $('#survey_feedback').attr('style', ''); /*初次点击，移出所以修改样式，重置状态*/
        $('#survey_feedback').removeClass('score').addClass(isLogin ? 'detail' : 'detailNo');
        clearTimeout(timers);
        clearTimeout(timers1);
        const data = querySurveyStrategy(module);
        if (data.isEnabled == 'Y') {
            if (data.isRepeatedAllowed == 'N' && data.status.indexOf(2) > -1) {/*isRepeatedAllowed 为N，且status包含 2*/
                sendMessage({type: 'hasPopped', viewModel: survey.viewModel}, 1, iframeEle, survey.target);/*用户已作答过*/
            } else {
                sendMessage({type: 'show', viewModel: survey.viewModel}, 1, iframeEle, survey.target);/*正常展示*/
            }
        } else if (data.isEnabled == 'N') {
            sendMessage({type: 'noSurvey', viewModel: survey.viewModel}, 1, iframeEle, survey.target);/*问卷不存在*/
        }
        $('#survey_feedback').fadeIn(200);
        setTimeout(()=>{
            // 获取iframe的内容文档
            var iframeDoc = iframeEle.contentDocument || iframeEle.contentWindow.document;
            let survey_page_content =  iframeDoc.getElementsByClassName('survey_page');
            if(!survey_page_content[0].textContent){
                $('#survey_feedback').attr('style', 'display:none');
            }
        },5000)
    }

}

// 帮助中心跳转
function jumpHelpCenter (){
    // 监听帮助中心按钮事件
    $('#feedDiv').find('.right-con .right_item.helpCenter').off('click').on('click',function(e){
        stopBubble(e);
        let targetHref = location.host.indexOf('localhost') > -1 ? location.protocol + '//localhost.huawei.com:8083' : location.protocol + '//' + location.host;
        location.href = checkXssHTML(`${targetHref}/info-finder/vue/help-center/${$('#lang').val()}/${$('#domainStr').val()}/${$('#surveyModule').val()?$('#surveyModule').val():$('#topicType').val()}`)
    })
}


/** 从子页面获取数据 */ function getMessage() {
    window.addEventListener("message", function (e) {
        const target = location.host.indexOf('localhost') > -1 ? location.protocol + '//localhost.huawei.com:8083' : location.protocol + '//' + location.host;
        if (e.origin != target) {
            return;
        }
        if (e.data.type === 'close') {
            if (e.data.isIgnore) {
                const thisModule = survey.module.indexOf('speccenter') > -1 ? 'speccenter' : survey.module;
                setInfoCookie('IgnoreSurvey_' + thisModule, true, 7);
            }
            $('#survey_feedback').fadeOut(200);
        } else if (e.data.type === 'changeSize') {
            if (e.data.height <= window.innerHeight) {
                $('#survey_feedback').height(e.data.height);
                $('#survey_iframe').attr('scrolling', 'no');
                $('#survey_feedback').css('--height', e.data.height + 'px');
                if (290 + e.data.height / 2 > window.innerHeight || e.data.height > 580) { /*首页详情页*/
                    $('#survey_feedback').css('bottom', 0);
                } else {
                    $('#survey_feedback').css('bottom', 'calc(290px - var(--height)/2)');
                }
            } else {
                $('#survey_iframe').attr('scrolling', 'yes');
                $('#survey_feedback').height(window.innerHeight);
                $('#survey_feedback').css('--height', window.innerHeight + 'px');
                $('#survey_feedback').css('bottom', 0);
            }
            if (e.data.width) {
                $('#survey_feedback').width(e.data.width);
            }
        } else if (e.data.type === 'forLogin') {
            forLogin()
        } else if (e.data.type === 'changeStyle') {
            $('#survey_feedback').height(100);
            $('#survey_feedback').css('--height', '100px');
            $('#survey_feedback').css('right', '0px');
            $('#survey_feedback').css('bottom', 'calc(290px - var(--height)/2)');
        } else if (e.data.type === 'closeBulletinTips') {
            if (e.data.isVisible) {
                $('#bullentins-tip').show();
                return;
            }
            $('#bullentins-tip').hide();
        } else if (e.data.type === 'closeEmergency') {
            if (e.data.isVisible) {
                $('#bullentins-popWindow').show();
                return;
            }
            $('#bullentins-popWindow').hide();
        }else if (e.data.type === 'closeSubscribe') {
            if (e.data.isVisible) {
                $('#email_subscribe').show();
                return;
            }
            $('#email_subscribe').fadeOut(200);
        }else if (e.data.type === 'privacy') {
            if (e.data.val == 'declaration') {
            // 跳转隐私签署querySign页
                goToPrivacy()
            }else{
                const isLogin = $('#isLogin').val() === 'true' ? true : false;
                // 跳转隐私签署页
                localStorage.setItem('check',true)
                localStorage.setItem('isSubscribePop',true)
                    forLogin()
            }
        }else if(e.data.type === 'forLogin'){
            forLogin()
        }else if(e.data.type === 'jumpSearchDetail'){
            let targetHref = location.host.indexOf('localhost') > -1 ? location.protocol + '//localhost.huawei.com:8081' : location.protocol + '//' + location.host;
            location.href = checkXssHTML(`${targetHref}/info-finder/${$('#domainStr').val()}search/${$('#lang').val()}?keyword=${e.data.data.keyword}&area=${e.data.data.area}&scene=${e.data.data.scene}`)
        }
    }, false);
}

function getQuerySurveyTime(module) {
    const data = querySurveyStrategy(module);
    if (!data) {
        return;
    }
    survey.statusData = data;
    let str = 'false';
    const viewModel = data.viewModel;
    survey.viewModel = viewModel;
    if (viewModel === 'nps') {
        $('#survey_feedback').addClass('NpsItem');
    } else if (viewModel === 'reasonclassified') {
        $('#survey_feedback').addClass('ReasonItem');
    } else if (viewModel === 'reasonnotclassified') {
        $('#survey_feedback').addClass('ReasonItem');
    }
    if (data.isEnabled == 'Y') {
        $('.right-layer .right-con .right_item.surVey').show();
        if (data.activePopping) {
            if (data.isRepeatedAllowed == 'N' && data.status.indexOf(2) == -1) {/*isRepeatedAllowed 为N，且status不包含 2判断是否弹出*/
                if (data.status.indexOf(1) == -1) {
                    str = data.retentionTime;
                }
            } else if (data.isRepeatedAllowed == 'Y') {/*黎丹2023.7.11要求--isRepeatedAllowed 为Y的时候不管是1还是2都不弹*/
                if (data.status.indexOf(1) == -1 && data.status.indexOf(2) == -1) {
                    str = data.retentionTime;
                }
            }
        }
    } else {
        $('.right-layer .right-con .right_item.surVey').hide();
    }
    return str;
}

function querySurveyStrategy(module, isAsync, callback) {
    let datas = {};
    $.ajax({
        contentType: 'application/json;charset=utf-8',
        type: "Post",
        url: $('#common-chang-path').val() + '/survey/querySurveyStrategy',
        dataType: "json",
        async: isAsync ? true : false,
        cache: false,
        headers: {},
        data: JSON.stringify({lang: $('#lang').val(), domain: $('#domain').val(), module: module}),
        success: function (res) {
            var data = res.data;
            if (!data || data.length == 0) {
                datas = {};
            }
            datas = data;
            if (typeof callback == 'function') {
                i18n_detail.statusData = data;
                callback()
            }
        },
        error: function (err) {
        }
    });
    return datas;
}
function resetIframeSet(type) {/*重置iframe窗口设置*/
    $('#survey_feedback').hide();
    let _height = '220px';
    if ($("#surveyModule").val() === 'encyclopedia-portal') {
        _height = '180px'
    }
    survey.isFirst = true;
    if (type === 'star') {
        $('#survey_feedback').css({height: _height, width: '260px', '--height': _height, right: '100px'});
    } else if (type === 'Nps') {
        $('#survey_feedback').css({height: '270px', width: '520px', '--height': '270px', right: '100px'});
    }
}

/**获取0~1之间的随机数，Math.random()等价表达式 */ function getRandomValues() {
    const crypto = window.crypto || window.msCrypto;
    const randomBuffer = new Uint32Array(1);
    return crypto.getRandomValues(randomBuffer)[0] / Math.pow(2, 32);
}

/**生成一个范围内的随机数 */ function getRandomIntInclusive(min, max) {
    let randomNumber = getRandomValues();
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(randomNumber * (max - min)) + min;
}/*Cookie写入*/
function setInfoCookie(name, value, expires, path, domain, secure) {
    var today = new Date();
    today.setTime(today.getTime());
    if (expires) {
        expires *= 86400000;
    }
    var expires_date = new Date(today.getTime() + (expires));
    document.cookie = name + "=" + encodeURIComponent(value) + ';' + (expires ? ";expires=" + expires_date.toGMTString() : "") + (path ? ";path=" + path : "") + (domain ? ";domain=" + domain : "") + (secure ? ";secure" : "");
}/*获取指定cookie的值*/
function getInfoCookie(name) {
    var cookies = document.cookie.split(';');
    var arr = '';
    for (var i = 0; i < cookies.length; i++) {
        var c = cookies[i].split('=');
        if (c[0].replace(' ', '') == name) {
            arr = c[1];
            return arr;
        }
    }
    return arr;
}/*删除cookies*/
function delInfoCookie(name, setWhichOne) {
    var exp = new Date();
    exp.setTime(exp.getTime() - 1);
    var cval = getInfoCookie(name);
    if (cval) {
        setInfoCookie('lang', 365, '/', '');
    }
}

/**校验用户输入 */ function filterXssString(Str) {
    const newStr = Str.toLowerCase().replace(/%3C/ig, '<').replace(/%3E/ig, '>');
    if (typeof newStr == "string" && newStr) {
        var exp = new RegExp("\\b(document|onload|eval|script|img|svg|onerror|javascript|alert)\\b");
        var exp1 = new RegExp("<input[^>]*");
        if (exp.test(newStr) || exp1.test(newStr)) {
            alert($('#lang').val() === 'zh' ? "输入不合法！" : 'Invalid input!');
            return true;
        }
    }
    return false;
}

/**校验用户输出 */ function checkXssInputOut(str) {
    const newStr = str.toLowerCase().replace(/%3C/ig, '<').replace(/%3E/ig, '>');
    if (typeof newStr == "string" && newStr) {
        var exp = new RegExp("\\b(document|onload|eval|script|img|svg|onerror|javascript|alert)\\b");
        var exp1 = new RegExp("<input[^>]*");
        if (exp.test(newStr) || exp1.test(newStr)) {
            return newStr.replace(/</g, '@').replace(/>/g, '@');
        }
    }
    return str;
}

function checkUemEnable(v, l) {
    const lang = $('#lang').val() ? $('#lang').val() : l;
    if (!$('#modelNum').val() == 1 && lang != 'zh') { /* 需要cookie签署的，先停止uem采集*/
        window.hwa("setEnable", getCookieCheck(v, lang));
    }
}

function getCookieCheck(v, l) {
    const version = v ? v : cookieOptions.version;
    const ls = l ? l : cookieOptions.language;
    const lang = ls === 'en' ? 'ENG' : 'CHN';
    const keys = "hw_privacy_cookie_esddp__".concat(lang, "_").concat(version);
    const isCookies = decodeURIComponent(getInfoCookie(keys));
    return isCookies && isCookies[0] === '{' && JSON.parse(isCookies).Analytics ? true : false
}

function callbackError(XMLHttpRequest, textStatus, errorThrown) {
    let tips = $('#reLoginTips').val();
    if (XMLHttpRequest.readyState == 0 && XMLHttpRequest.status == 0 && textStatus == 'error') {
        tips = "登录状态已经失效，请重新登录";
    } else {
        if (XMLHttpRequest.status === 401) {
            console.log($('#lang').val())
            tips = $('#lang').val()=='zh'?"登录超时，请重新登录":'You have not logged in or the login times out. Please log in again.';
        }

    }
    let opt = {
        messages: tips, isCancel: 'Y', onDialog: function () {
            forLogin();
        }
    };
    if (XMLHttpRequest.status != 403) {
        $('#common-reLogin-pop').hardDialogBox(opt);
    }
}

function getCollapse(){
    $('#feedDiv').find('.right_item.fold').off('click').on('click', function (e) {
        stopBubble(e);
        const $con=$('#feedDiv').find('.right-con');
        if($con.hasClass('collapses')){ //收起状态
            $(this).find('.text').text($('#right_collapse').val());
            $con.removeClass('collapses');
        }else{
            $(this).find('.text').text($('#right_expand').val());
            $con.addClass('collapses');
        }
    })
}

let timer3;
(function () {
    $('.headerSearch').off("click").on("click",function(e){
        $('#header-searchPop').empty();
        let iframeSrc = location.href.indexOf('localhost') > -1 ?`http://localhost.huawei.com:8083/searchBoxForIndex/${$('#lang').val()}/${$('#domainStr').val()}/index`:location.protocol+'//'+location.host+`/info-finder/vue/searchBoxForIndex/${$('#lang').val()}/${$('#domainStr').val()}/index`
        $('#header-searchPop').append(checkXssHTML(`<div class="header-searchPop-close"><span class="header-searchPop-close-icon"></span></div><iframe class="header-searchPop-iframe" src="${iframeSrc}" scrolling="no" frameborder="0" width="100%" height="100%"></iframe>`))
        let iframeEle = document.querySelector('.header-searchPop-iframe')
        iframeEle.onload = function(){
            timer3 = setTimeout(function(){
                $('#header-searchPop').css('display','flex')
                $('#header-searchPop-mask').attr('style','')
                $('.header-searchPop-close-icon').off("click").on("click",function(e){
                    $('#header-searchPop').css('display','none')
                    $('#header-searchPop-mask').css('display','none')
                })
            },100)
        }
    })
    $('#header-searchPop-mask').off("click").on("click",function(e){
        $('#header-searchPop-mask').css('display','none')
        $('#header-searchPop').css('display','none')
    })

    // 监听页面滚动事件
    window.addEventListener('scroll',function(){
        let height = document.documentElement.scrollTop+document.body.scrollTop
        if(height >= 40){
            $('#header-searchPop').css('position','sticky')
            $('#header-searchPop').css('top','59px')
        }else{
            $('#header-searchPop').css('position','absolute')
            $('#header-searchPop').css('top','auto')
        }
    })

})();

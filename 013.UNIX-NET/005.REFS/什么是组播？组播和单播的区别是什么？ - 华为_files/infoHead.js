var commonHead = {userLevel: -1,/*鉴权*/};
$(function () {
    checkHeadNav();
    addChangLists();
    var domainStr = $('#domainStr').val();
    var isMobile = isOpenSearchMobile();
    if (isMobile) {/*移动端的初始化*/
        mobileNavInit();
        $('#iphone-nav').off('click').on('click', function () {
            $('.ip-footer ul').hide();
        });
        $('.header_boxshadow').addClass('device_shadow');
    } else if (window.innerWidth <= 1024) {
        mobileNavInit();
        $('#iphone-nav').off('click').on('click', function () {
            $('.ip-footer ul').hide();
        });
        $('.header_boxshadow').addClass('device_shadow');
    } else {/*pc端的语言初始化*/
        showLangStr();
        setPosition();
        $('.header_boxshadow').removeClass('device_shadow');
    }
    $('body').off('click').on('click', function () {
        $('.header-right .lang ul').hide();
    });
});/*阻止事件冒泡*/
function stopBubble(e) {
    if (e && e.stopPropagation) e.stopPropagation(); else {/*IE的方式来取消事件冒泡*/
        window.event.cancelBubble = true;
    }
}

function showLangStr() {
    var pathname = window.location.pathname;
    var domainStr = $('#domainStr').val();
    var thisPage = $('#thisPage').val();
    var list = pathname.split('/');
    var getLang = $('#lang').val();
    var changLangList = $('#headerTop').find('.new-change-lang');
    for (var i = 0; i < changLangList.length; i++) {
        var minLang = $(changLangList[i]).attr('lang');
        if (getLang == minLang) {
            var textStr = $(changLangList[i]).text();
            $('#headerTop').find('.lang-name').text(textStr);
            $('#headerDIV').find('.carrier').find('.lang-name').text(textStr);
            break;
        }
    }
}/*切换语言的效果*/
function showlangSelect(e, t) {
    stopBubble(e);
    $('#headerTop').find('.lang-val').toggle();
    $('#iphone-nav').find('.lang-val').toggle();
    if ($('#survey_iframe').is(':visible')) {
        let iframeEle = document.getElementById('survey_iframe');
        sendMessage({type: 'close'}, 1, iframeEle);/*正常展示*/
    }
    if (window.innerWidth <= 1024) {
        if ($('#iphone-nav').find('.lang-val').is(':visible')) {
            $(t).find('i.select-down').css('transform', 'rotate(0)');
        } else $(t).find('i.select-down').css('transform', 'rotate(180deg)');
    } else if ($('#headerTop').find('.lang-val').is(':visible')) {
        $(t).find('i.select-down').css('transform', 'rotate(180deg)');
    } else $(t).find('i.select-down').css('transform', 'rotate(0)');
}/*移动端初始化*/
function mobileNavInit() {/*手机端的导航的切换*/
    $('#headerDIV .iphone-btn').off('click').on('click', function (event) {/*取消冒泡*/
        $('#iphone-nav').show();
    });
    $('#iphone-nav .ip-head span').off('click').on('click', function (event) {/*取消冒泡*/
        stopBubble(event);
        $('#iphone-nav').hide();
        $('#solution-modal-mobile').hide();
    });/*根据URL判断语言的显示*/
    mobileLangShow();
}/*移动端语言的显示*/
function mobileLangShow() {
    var getLang = $('#lang').val();
    var domainStr = $('#domainStr').val();
    var thisPage = $('#thisPage').val();
    var changLangList = $('#iphone-nav').find('.new-change-lang');
    for (var i = 0; i < changLangList.length; i++) {
        var minLang = $(changLangList[i]).attr('lang');
        if (getLang == minLang) {
            var textStr = $(changLangList[i]).text();
            $('#iphone-nav').find('.ip-chang-lang').text(textStr);
            $('#headerDIV').find('.carrier').find('.lang-name').text(textStr);
            break;
        }
    }
}/*判断是否为手机端*/
function isOpenSearchMobile() {
    if (navigator.userAgent.match(/(phone|pod|iPhone|iPod|ios|Android|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i) || window.innerWidth <= 768) return true;
    return false;
}/*HTML反编码*/
function HTMLDecode(value) {
    try {
        value = value.replace(/</g, "&lt;");
        value = value.replace(/>/g, "&gt;");
        var temp = document.createElement("div");
        temp.innerHTML = value;
        var output = temp.innerText || temp.textContent;
        if (output == "undefined") output = "";
        temp = null;
        return output;
    } catch (e) {
        return value;
    }
}

function enterpriseHTMLEncode(c) {
    var b = document.createElement("div");
    (b.textContent != null) ? (b.textContent = c) : (b.innerText = c);
    var a = b.innerHTML;
    b = null;
    return a;
}

function enterpriseHTMLDecode(c) {
    if (c == undefined) return "";
    var b = document.createElement("textarea");
    b.innerHTML = c;
    var a = b.innerText || b.textContent;
    b = null;
    return a;
}/* 判断浏览器是否支持sticky 属性*/
function isSticky() {
    var vendorList = ['', '-webkit-', '-ms-', '-moz-', '-o-'], vendorListLength = vendorList.length,
        stickyElement = document.createElement('div');
    for (var i = 0; i < vendorListLength; i++) {
        stickyElement.style.position = vendorList[i] + 'sticky';
        if (stickyElement.style.position !== '') return true;
    }
    return false;
};/* 设置元素的fixed属性*/
function divFixedTop(ele, top) {
    var scrollTop = $(window).scrollTop() - top;  /*滚动条的位置加上当前元素的fixed的top值(用来动态设置fixed的top值的触发点)*/
    var thisTop = $(ele).css("top").split('px')[0];  /*获取当前元素存储在元素属性当前的top值*/
    var margin_top = $(ele).css("margin-top").split('px')[0];  /*获取当前元素存储在元素dom属性上的margin-top值并截取掉除px的值*/
    var left = $(ele).offset().left;  /*获取元素静态时距离页面左边的偏移量,用来设置fixed的left值*/
    if (scrollTop >= thisTop - margin_top) {
        $(ele).css({'position': 'fixed', 'bottom': 'auto', 'margin-top': '0px', 'top': '0px', 'left': left + 'px'});
        $('.bannerContainer > img').css('top', '-90px');
        $('.search_infoCenter').css('marginTop', '140px');
        $('#detail-bannner').css('marginTop', '60px');
    } else if (scrollTop <= thisTop + margin_top) {
        $(ele).css({
            'position': 'relative',
            'margin-top': margin_top + 'px',  /*精确fixed的top值*/
            'top': '',
            'left': ''
        });
        $('.bannerContainer > img').css('top', '-90px');
        $('.search_infoCenter').css('marginTop', '90px');
        $('#detail-bannner').css('marginTop', '0')
    }
};/*如果当前浏览器兼容position: sticky,属性就返回*/
function setPosition() {
    var sticky = isSticky();
    if (sticky) {
        return;/* 否则就动态js设置元素的fixed触发值*/
    } else {
        $('#headerDIV').css('position', 'relative');/*每次滚动就监测调用divViewHeight()方法,判断是否到达触发点*/
        $(window).on('scroll', function () {
            divFixedTop('#headerDIV', 25);
        });
    }
}

function checkHeadNav() {
    var pathName = window.location.pathname;
    if (pathName.indexOf('encyclopedia') > -1) {/*百科*/
        $('#headerDIV .header-mid .nav.encyclopedia').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.encyclopedia').addClass('active');
        var navigation = $('#headerDIV .header-mid .nav.encyclopedia').html().split('<')[0];
        $('#headerDIV .header-left>div').text(navigation);
        $('.header-right .chang-version').hide();
    } else if (pathName.indexOf('search-center') > -1) {/*信息速查*/
        $('#headerDIV .header-mid .nav.search-center').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.search-center').addClass('active');
        $('.header-right .chang-version').hide();
        var navigation = $('#headerDIV .header-mid .nav.search-center').html().split('<')[0];
        $('#headerDIV .header-left>div').text(navigation);
    } else if (pathName.indexOf('/info-finder/solution') > -1) {/*解决方案*/
        $('.header-right .chang-version').hide();
        $('#uem_test_code').val('PDBAA1385C5771E');
        $('#uem_code').val('PDBAA1382BD3AE6');
    } else if (pathName.indexOf('/bookshelf') > -1) {/*产品文档*/
        $('#uem_test_code').val('PDBAA1386F7FAF1');
        $('#uem_code').val('PDBAA1382CB52C5');
        if (location.search === '?sol=new-products') return $('#headerDIV .header-mid .nav.documentation').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.documentation').addClass('active');
        $('.header-right .chang-version').hide();
        var navigation = $('#headerDIV .header-mid .nav.documentation').html().split('<')[0];
        $('#headerDIV .header-left>div').text(navigation);
    } else if (pathName.indexOf('/technology') > -1) {/*技术专区*/
        $('#headerDIV .header-mid .nav.technology').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.technology').addClass('active');
        $('.header-right .chang-version').hide();
        $('#uem_test_code').val('PDBAA1384A11705');
        $('#uem_code').val('PDBAA1384027B92');
        var navigation = $('#headerDIV .header-mid .nav.technology').html().split('<')[0];
        $('#headerDIV .header-left>div').text(navigation);
    } else if (pathName.indexOf('/maintenance') > -1) {/*维护专区*/
        $('#headerDIV .header-mid .nav.maintenance').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.maintenance').addClass('active');
        $('.header-right .chang-version').hide();
        $('#uem_test_code').val('PDBAA1381687559');
        $('#uem_code').val('PDBAA138556F24D');
        var navigation = $('#headerDIV .header-mid .nav.maintenance').html().split('<')[0];
        $('#headerDIV .header-left>div').text(navigation);
    } else if (pathName.indexOf('/engineer') > -1) {/*工程师专区*/
        $('#headerDIV .header-mid .nav.huaweiengineers').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.huaweiengineers').addClass('active');
        $('.header-right .chang-version').hide();
        $('#uem_test_code').val('PDBAA13856B0EE6');
        $('#uem_code').val('PDBAA1384B99C1F');
        var navigation = $('#headerDIV .header-mid .nav.huaweiengineers').html().split('<')[0];
        $('#headerDIV .header-left>div').text(navigation);
    } else if (pathName.indexOf('/multimedia') > -1) {/*多媒体专区*/
        $('#headerDIV .header-mid .nav.video').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.video').addClass('active');
        var navigation = $('#headerDIV .header-mid .nav.video').html().split('<')[0];
        $('#headerDIV .header-left>div').text(navigation);
    } else if (pathName.indexOf('recomproduct') > -1 || pathName.indexOf('bid/autoReply') > -1) {/*智能选型*/
        $('#headerDIV .header-mid .nav.bidding').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.bidding').addClass('active');
        $('.header-right .chang-version').hide();
        var navigation = $('#headerDIV .header-mid .nav.bidding').html().split('<')[0];
        $('#headerDIV .header-left>div').text(navigation);
    } else if ((pathName.indexOf('/tool') > -1 && $('#thisPage').val() == 'UXtools') || (pathName.indexOf('/license') > -1 && $('#thisPage').val() == 'UXtools')) {/*独立小工具页面*/
        $('#headerDIV .header-mid .nav.search-center').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.search-center').addClass('active');
        $('.header-right .chang-version').hide();
    } else {/*开放式搜索首页*/
        $('#headerDIV .header-mid .nav.openSearch').siblings().removeClass('active');
        $('#headerDIV .header-mid .nav.openSearch').addClass('active');
        $('.header-right .chang-version').hide();/*切换旧版本不再显示*/
        $('#headerDIV .header-left>div').text('Info-Finder');
    }
}

function addChangLists() {
    var pathName = window.location.pathname;
    var isMobile = isOpenSearchMobile();
    if (pathName.indexOf('search-center') > -1 && location.href.indexOf('style=old') > -1) {
        var langStr = '<li class="new-change-lang" lang="zh">中文</li>\n' + '<li class="new-change-lang" lang="en">English</li>\n' + '<li class="new-change-lang" lang="de">Deutsch</li>\n' + '<li class="new-change-lang" lang="es">Español</li>\n' + '<li class="new-change-lang" lang="fr">Français</li>\n' + '<li class="new-change-lang" lang="it">Italiano</li>\n' + '<li class="new-change-lang" lang="jp">日本語</li>\n' + '<li class="new-change-lang" lang="br">Português</li>\n' + '<li class="new-change-lang" lang="ru">Русский</li>';
        $('#headerTop .header-right .lang ul.lang-val').empty();
        $('#headerTop .header-right .lang ul.lang-val').append(langStr);
        $('#iphone-nav .ip-footer ul.lang-val').empty();
        $('#iphone-nav .ip-footer ul.lang-val').append(langStr);
        $('#iphone-nav .ip-footer ul.lang-val').css('top', '-274px');
        $('#headerDIV .header-right .lang ul.lang-val').empty();
        $('#headerDIV .header-right .lang ul.lang-val').append(langStr);
    }
}/*获取用户等级d的公共方法*/
function getCommonUserLevel() {
    let userLevel = commonHead.userLevel;
    $.ajax({
        type: "POST",
        url: $('#common-chang-path').val() + "/specification/getUserLevel",
        data: {domain: $('#domain').val()},
        traditional: true,
        dataType: 'json',
        async: false,
        success: function (data) {
            userLevel = data;
        }
    });
    return userLevel;
}

function roleChangeReLogin() {/* 角色发生变化，强制重新登录*/
    if (commonHead.userLevel != getCommonUserLevel()) {
        let tips = $('#reLoginTips').val();
        let opt = {
            messages: tips, isCancel: 'Y', onDialog: function () {
                forLogin();
            }
        };
        $('#common-reLogin-pop').hardDialogBox(opt);
        return true;
    }
    return false;
}

/** 提权弹框 */ function changeRoleLevel(isLogin, state, userId) {
    let sureTxt, message;
    let onSure = function () {
    };
    if (!isLogin) {/*未登录*/
        $('#change-userLevel-tip').css('right', '-20px');
        message = $('#dialog-message-nologin').val();
        sureTxt = $('#dialog-btn-login').val();
        onSure = function () {
            localStorage.setItem('isClose', '0');
            forLogin();
        }
    } else if (state == 1) {/*提权*/
        if (localStorage.getItem(window.btoa(userId + '_state')) == "1") {
            return;/*相同账号，session有效期内不再弹框*/
        }
        message = $('#dialog-message-uplevel').val();
        sureTxt = $('#dialog-btn-uplevel').val();
        onSure = function () {
            localStorage.setItem(window.btoa(userId + '_state'), "1");
            localStorage.setItem('isClose', '0');
            upLevel();
        }
    } else if (state == 2) {/*匿名用户*/
        let time = new Date();/*匿名用户操作存储15min*/
        let timeN = time - new Date(decodeURI(localStorage.getItem('unKnow')));
        if (timeN <= 5 * 60 * 1000) {
            return;/*5分钟内匿名用户不再提示*/
        }
        message = $('#dialog-message-unknow').val();
        sureTxt = $('#dialog-btn-go').val();
        onSure = function () {
            localStorage.setItem('unKnow', encodeURI(time.toString()));
            localStorage.setItem('isClose', '0');
            const newH = window.open($('#dialog-support-url').val());
            newH.opener = null;
        }
    } else return;
    let onCancel = function () {
        let $userId;
        if (isLogin) {
            $userId = $('.header-right span').find('.user-id');
        } else $userId = $('.header-right span').find('#login');
        if ($userId.next('i.notice').length == 0) {/*增加感叹号悬浮*/
            let tipStr = '<i class="notice"><div class="dialogTip" id="hover-userLevel-tip" style="display: none"></div></i>';
            $userId.after(tipStr);
            let options1 = {
                message: message, sureTxt: sureTxt, isTime: false, onSure: function () {
                    if ($userId.next('i.notice').length > 0) $userId.next('i.notice').remove();
                    onSure();
                },
            };
            $('#hover-userLevel-tip').dialogTip(options1);
            initTipsHover();
            localStorage.setItem('isClose', '1');
        }
    };
    if (localStorage.getItem('isClose') == "1") {/*用户已经关闭过该弹框，不再弹出提示，留悬浮感叹号*/
        onCancel();
        return;
    }
    let options = {
        message: message, sureTxt: sureTxt, onSure: function () {
            onSure();
        }, onCancel: function () {
            onCancel();
        }
    };
    $('#change-userLevel-tip').dialogTip(options);
}

/** 发送提权邮件 */ function upLevel() {
    let pageTitle = $('#dialog-support-email-title').val();
    let abstract = $('#dialog-support-email-abstract').val();
    let to = $('#dialog-support-email-to').val();
    let copy = $('#dialog-support-email-cc').val();
    let url = 'mailto:' + to + '?subject=' + pageTitle + '&cc=' + copy + '&body=' + abstract;
    const newH = window.open(url);
    newH.opener = null;
}

/*** 通知感叹号悬浮出现面板 */ function initTipsHover() {
    let $notice = $('.header-right i.notice');
    $notice.on({
        mouseover: function (e) {
            stopBubble(e);
            $('#hover-userLevel-tip').css({visibility: 'visible', opacity: 1});
        }, mouseout: function (e) {
            stopBubble(e);
            $('#hover-userLevel-tip').css({visibility: 'hidden', opacity: 0});
        }
    });
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
}

/** 销售区域隔离提示-相关推荐数据获取-仅华为工程师可见 */ function getQuerySupportInfo(keyword, toolType, seriesPbiId, subSeriesPbiId, modelPbiId, isAsync, callback) {
    let url = '';
    $.ajax({
        contentType: 'application/json;charset=utf-8',
        type: "Post",
        url: $('#common-chang-path').val() + "/openSearch/querySupportInfo",
        dataType: "json",
        async: isAsync,
        cache: false,
        data: JSON.stringify({
            keyword: keyword,
            language: $('#lang').val(),
            domain: $('#domain').val(),
            toolType: toolType,
            seriesPbiId: seriesPbiId,
            subSeriesPbiId: subSeriesPbiId,
            modelPbiId: modelPbiId
        }),
        success: function (res) {
            var data = res.data;
            if (data && !$.isEmptyObject(data) && data.isCurrentSupport == 'N') {
                let domain = data.domain;
                let lang = data.language;
                if (lang && (domain == "1" || domain == "0")) if (toolType) {/*工具*/
                    url = getToolSearchUrl(domain, lang);
                } else {/*开放式搜索*/
                    url = getDetailSearchUrl(domain, lang, keyword, 'switch');
                    i18n_detail.switchUrl = url;
                }
            }
            if (typeof callback === "function") {
                callback();     /*调用传入的回调函数*/
            }
        },
        error: function (err) {
        }
    });
    if (!isAsync) return url;
};/** 顶部弹出公告中心气泡 */ let bullets_target = location.host.indexOf('localhost') > -1 ? location.protocol + '//localhost.huawei.com:8083' : location.protocol + '//' + location.host;
let emergencyTimer, tipTimer;

function getBulletinsTips(isLogin) {
    if ($('#isBulltinsHide').val() == 1 || $("#hidden_o3").val() == 1) {
        $('.bulletin-nav').remove();
        return;
    }
    if (isMobile()) {/*暂时移动端不支持*/
        return;
    }
    $('.bulletin-nav').show();
    $('#bullentins-tip').append(checkXssHTML('<iframe id="bulletin_iframe"  border="0" scrolling="no" width="270" height="35"></iframe>'));
    let iframeEle = document.getElementById('bulletin_iframe');
    iframeEle.src = "/info-finder/vue/bulletinsTip/" + $('#lang').val() + "/" + $('#domainStr').val() + "/index";
    clearTimeout(tipTimer);
    iframeEle.onload = function (e) {
        tipTimer = setTimeout(function () {
            sendMessage({module: $('#bulletins-module').val(), isLogin: isLogin}, 1, iframeEle, bullets_target);/*正常展示*/
        }, 300);
    };
    $('#bullentins-popWindow').append(checkXssHTML('<iframe id="emergency_iframe"  border="0" scrolling="no" width="720" height="432"></iframe>'));
    let emergencyEle = document.getElementById('emergency_iframe');
    emergencyEle.src = "/info-finder/vue/emergency/" + $('#lang').val() + "/" + $('#domainStr').val() + "/index";
    clearTimeout(emergencyTimer);
    emergencyEle.onload = function (e) {
        emergencyTimer = setTimeout(function () {
            sendMessage({module: $('#bulletins-module').val(), isLogin: isLogin}, 1, emergencyEle, bullets_target);/*正常展示*/
        }, 300);
    }
}
var EHead = EHead || function () {
};
EHead.prototype.init = function () {
    var c = this;
    var b = $.Deferred();
    c.language = null;
    c.loginPreURL = "";
    c.logoutPreURL = "";
    c.isLogin = null;
    c.links = null;
    c.enterpriseURL = "";
    c.supportURL = "";
    c.requestURL = "";
    c.USER_ID = "";
    c.USER_EMAIL = "";
    c.userLevel = "";
    var a = $("#language").val();
    $("html").addClass("LANG-" + a);
    var comPath = $('#common-chang-path').val();
    $.ajax({
        type: "POST",
        url: comPath + "/user/getHeadInfo",
        dataType: "json",
        cache: false,
        async: false,
        data: {headLang: a, domain: $('#domain').val()},
        success: function (d) {
            if (!d) {
                return
            }
            if (location.hostname.indexOf('localhost') == -1 && d.isLogin && queryPrivacyCheck('') != 'true') {
                const lang = $('#language').val();
                const domainStr = $('#domainStr').val();
                const url = $('#common-chang-path').val() + '/vue/' + lang + '/' + domainStr + '/privacy';
                localStorage.setItem('beforePath', location.href);
                localStorage.setItem('isFromS', '1');
                location.href = checkXssHTML(url);
                return;
            }
            let levelNum = d.userLevel;
            commonHead.userLevel = levelNum;
            commonHead.userId = d.USER_ID;
            if (levelNum == '6') {
                $('#headerDIV').find('.header-con .huaweiengineers').show();
                $('#iphone-nav').find('.ip-nav .huaweiengineers').show();
            }
            c.language = d.language;
            c.loginPreURL = d.loginPreURL;
            c.logoutPreURL = d.logoutPreURL;
            c.isLogin = d.isLogin;
            c.links = d.links;
            c.enterpriseURL = d.enterpriseURL;
            c.supportURL = d.supportURL;
            c.USER_ID = d.USER_ID;
            c.USER_EMAIL = d.USER_EMAIL;
            c.userLevel = d.userLevel;
            c.state = d.state;
            $('#isLogin').val(d.isLogin);
            $('#loginPreURL').val(d.loginPreURL);/*初始化页头完毕后向iframe传输是否登录*/
            getBulletinsTips(d.isLogin);
            if (!d.isLogin) {
                $('#bullentins-tip').addClass('noLogin');
            }
            if($("#hidden_o3").val() != 1){
                const isPop = getSubscribe($('#surveyModule').val(),d.isIgnoreSubscribe,d.userLevel)
            }
 if(location.search === '?sol=new-products'){
                $('#surveyModule').val('new-products')
            }
            // 判断是否显示帮助中心
            $.ajax({
                contentType: 'application/json;charset=utf-8',
                type: "POST",
                url: comPath + "/helpCenter/queryDetailInfoByParam",
                dataType: "json",
                cache: false,
                async: true,
                data: JSON.stringify({language: $('#lang').val(), domain: $('#domain').val(),urlName:$('#surveyModule').val()?$('#surveyModule').val():$('#topicType').val()}),
                success: function (res) {
                    if(res && res.data && res.data.length){
                        // 帮助中心调用,监控点击事件
                        $('#feedDiv').find('.helpCenter').show()
                        jumpHelpCenter()
                    }else{
                        $('#feedDiv').find('.helpCenter').hide()
                    }
                }
            })
            // 判断邮件订阅是否弹出
        },
        error: function () {
            return
        },
        complete: function (e, d) {
            b.resolve(d)
        }
    });
    return b
};
EHead.prototype.forLogin = function (keyword) {
    if (!this.loginPreURL) {
        return;
    }
    infofinderLogout();
    var domainStr = $('#domainStr').val();
    let newHref = checkXssUrl(location.href);
    if (keyword) {
        let keys = getHrefkeyword();
        if (keys) {
            newHref = newHref.replace(keys, keyword);
        }
    }
    if (domainStr == 'carrier' && $('#thisPage').val() == 'infoFinder') {
        newHref = newHref.replace('.html', '');
        window.location.href = this.loginPreURL + encodeURIComponent(newHref.replace(/\(/g, '%28').replace(/\)/g, '%29').replace(/'/g, '%27'))
    } else {
        window.location.href = this.loginPreURL + encodeURIComponent(newHref.replace(/\(/g, '%28').replace(/\)/g, '%29').replace(/'/g, '%27'))
    }
};
EHead.prototype.forLogout = function () {
    if (!this.logoutPreURL) {
        return
    }
    infofinderLogout();/*产品智能选型退出登录回到选型首页*/
    if ($('#thisPage').val() == 'bidding') {
        let host = location.host;
        let path = $('#common-chang-path').val();
        let lang = $('#lang').val();
        let domainStr = $('#domainStr').val();
        let headHttp = location.protocol + '//';
        let IndexHref = checkXssUrl(headHttp + host + path + '/recomproduct/' + lang + '/index');
        if (lang === 'zh' || location.href.indexOf('quick') === -1) {
            window.location.href = this.supportURL + this.logoutPreURL + encodeURIComponent(IndexHref.replace(/\(/g, '%28').replace(/\)/g, '%29').replace(/'/g, '%27'));
        } else {
            window.location.href = this.supportURL + this.logoutPreURL + encodeURIComponent(checkXssUrl(location.href).replace(/\(/g, '%28').replace(/\)/g, '%29').replace(/'/g, '%27'));
        }
    } else {
        window.location.href = this.supportURL + this.logoutPreURL + encodeURIComponent(checkXssUrl(location.href).replace(/\(/g, '%28').replace(/\)/g, '%29').replace(/'/g, '%27'));
    }
};
EHead.prototype.getUserState = function () {
    var f = this;
    var c = f.isLogin;
    var a = f.links;
    var USER_ID = f.USER_ID;
    var i18nLogin = $('#i18n-login').val();
    var i18nLogout = $('#i18n-logout').val();
    var isMobile = isOpenSearchMobile();
    var pathName = window.location.pathname;
    var domainStr = $('#domainStr').val();
    var thisPage = $('#thisPage').val();
    var userName = getUserLever();
    if (isMobile || window.innerWidth <= 1024) {
        if (c) {
            $('#login-mobile').empty();
            var str = '<span class="user-id">' + USER_ID + userName + '</span><span class="login-line"></span>';
            var str2 = '<span id="ehead-login-mobile">' + i18nLogout + '</span>';
            $('#login-logout-mobile').prepend(str);
            $('#login-mobile').append(str2);
            $("#login-logout-mobile").on("click", function () {
                f.forLogout();
                return false
            });
        } else {/*产品智能选型中文和运营商模块未登录时跳转登录*/
            if (($('#domain').val() == "zh" && thisPage == 'bidding') || $('#domain').val() == 1) {
                f.forLogin();
                return false
            }
            $('#login-mobile').empty();
            var str = '<span id="ehead-login-mobile">' + i18nLogin + '</span>';
            $('#login-mobile').append(str);
            $("#ehead-login-mobile").on("click", function () {
                f.forLogin();
                return false
            });
        }
        return -1;
    }
    if (c) {
        $('#login').empty();
        var str = '<span class="user-id">' + USER_ID + userName + '</span><span class="login-line"></span>';
        var str2 = '<span id="ehead_logout">' + i18nLogout + '</span>';
        $('.login-logout').prepend(str);
        $('#login').append(str2);
        $("#ehead_logout").on("click", function () {
            f.forLogout();
            return false
        });
    } else {/*产品智能选型中文和运营商模块未登录时跳转登录*/
        if (($('#domain').val() == "zh" && thisPage == 'bidding') || $('#domain').val() == 1) {
            f.forLogin();
            return false;
        }
        $('#login').empty();
        var str = '<span id="ehead_login">' + i18nLogin + '</span>';
        $('#login').append(str);
        $("#ehead_login").on("click", function () {
            f.forLogin();
            return false
        });
    }
};
EHead.prototype.getUserLevel = function () {
    return {userLevel: eHeadObject.userLevel};
};
EHead.prototype.changeLang = function (g, whichPage) {
    var path = $("#common-chang-path").val();
    var thisHref = window.location.href;
    var pathName = window.location.pathname;
    var key = function (name) {
        var reg = new RegExp("([\\w-./?%&=]*)" + name + "=([^&]*)(&|$)"); /*构造一个含有目标参数的正则表达式对象*/
        var r = window.location.search.substr(1).match(reg);  /*匹配目标参数*/
        if (r != null) return unescape(r[2]);
        return null; /*返回参数值*/
    };
    if (whichPage != 'enterprisesearch' && whichPage != 'encyclopedia' && whichPage != 'carriersearch') {/*指定后缀切换语言时删除*/
        if (whichPage == 'tool' || whichPage == 'search-center') {
            if (key('componentType')) {/*url有componentType字段时，删除改后缀*/
                thisHref = replaceUrlNoKeys(false, 'componentType', false);
            }
        }
        var getLang = $('#lang').val();
        var hrefList = thisHref.split('/' + getLang + '/');
        var newHref = path + '/' + whichPage + '/' + g + '/' + hrefList[1];/*产品智能选型切换语言*/
        if (whichPage == 'recomproduct') {
            var permissionObj = EHead.prototype.getUserLevel();
            if (g == 'en') {
                if (thisHref.indexOf('quick') > -1) {
                    if (permissionObj.userLevel != '4' && permissionObj.userLevel != '6') {
                        var newHref = path + '/recomproduct/' + g + '/index';
                    } else {
                        var newHref = thisHref.replace('/zh/', '/en/').split('?')[0];
                        ;
                    }
                } else if (thisHref.indexOf('auto') > -1) {
                    var newHref = path + '/recomproduct/' + g + '/index';
                } else {
                    var newHref = thisHref.replace('/zh/', '/en/').split('?')[0];
                }
            } else {
                if (thisHref.indexOf('quick') > -1 && permissionObj.userLevel != '4' && permissionObj.userLevel != '6') {
                    var newHref = path + '/recomproduct/' + g + '/index';
                } else {
                    var newHref = thisHref.replace('/en/', '/zh/').split('?')[0];
                }
            }
        }
        if ($('#thisPage').val() == 'document' || $('#thisPage').val() == 'solution') {
            var pduName = $('#pdu_name').val();
            if (pduName == 'IPv6+' && g != 'zh') {/*中英文换叫法*/
                pduName == 'IPE'
            } else if (pduName == 'IPE' && g == 'zh') {
                pduName == 'IPv6+'
            }
            if ($('#thisPage').val() == 'document') {/*用来配置开放式搜索运营商书架页面*/
                var newHref = path + '/' + $('#thisPage').val() + '/' + whichPage + '/' + g + '/' + pduName + '.html';
            }
            if ($('#thisPage').val() == 'solution') {
                var newHref = path + '/' + $('#thisPage').val() + '/' + g + '/' + hrefList[1];
            }
        }
    } else {/*匹配搜索栏关键字*/
        var searchVal = key('keyword');/*当搜索栏有关键字时切换中英文跳转回首页*/
        var lang = $('#lang').val();
        var domain = $('#domain').val();
        if (searchVal && g != lang) {
            if (whichPage == 'enterprisesearch') {
                var newHref = path + '/' + g + '/enterprise/index'
            } else if (whichPage == 'carriersearch') {
                var newHref = path + '/' + g + '/carrier/index'
            }
        } else if (g == lang) {
            var newHref = window.location.href;
        } else {
            if(domain== '1'){
                var newHref = path + '/' + whichPage + '/' + g + '/carrier/index.html'
            }else {
                var newHref = path + '/' + whichPage + '/' + g + '/index.html'
            }
        }
    }

    function a(h) {
        if (null == h || "" == h) {
            return
        }
        if (whichPage) {
            location.href = checkXssUrl(newHref);
        } else {
            location.href == f ? location.reload() : location.href = checkXssUrl(f)
        }
    }

    if (!g) {
        return
    }
    var f = location.href;
    var e = "^(http[s]?://)([^/]*/)([^/]*/)([a-z]{2})($|/)(.*)?$";
    var c = new RegExp(e);
    if (c.test(f)) {
        f = f.replace(c, "$1$2$3" + g + "$5$6")
    }
    var d = "(\\?lang=|&lang=)([a-z]{2})(&?)";
    var b = new RegExp(d);
    if (b.test(f)) {
        f = f.replace(b, "$1" + g + "$3")
    }
    a(g)
};
EHead.prototype.initListener = function () {
    var _this = this;
    $('#headerTop').find('.new-change-lang').off('click').on('click', function (e) {
        stopBubble(e);
        var textVal = $(this).text();
        var a = $(this).attr("lang");
        var pathName = window.location.pathname;
        var getEnDataFromGraph = $('#getEnDataFromGraph').val();
        if ((pathName.indexOf("enterprisesearch") >= 0 || pathName.indexOf("carriersearch") >= 0) && a != 'zh' && getEnDataFromGraph == 'N') {
            var keyword = $('#search-inputbox').val();
            var protocol = window.location.protocol;
            var hostName = window.location.host;
            var rootName = $("#common-chang-path").val();
            var domainStr = $('#domainStr').val();
            var uniqueUrl = $("#uniqueUrl").val();
            var lang = 'en';
            var newHref = protocol + "//" + hostName + rootName + "/" + lang + '/' + domainStr + '/index';/* 跳转*/
            window.location.href = newHref;
        } else {
            var domainStr = $('#domainStr').val();
            if (pathName.indexOf('encyclopedia') > -1) {
                _this.changeLang(a, 'encyclopedia');
            } else if (pathName.indexOf('search-center') > -1) {
                _this.changeLang(a, 'search-center');
            } else if (pathName.indexOf('enterprisesearch') > -1) {
                _this.changeLang(a, 'enterprisesearch');
            } else if (pathName.indexOf('carriersearch') > -1) {
                _this.changeLang(a, 'carriersearch');
            } else if ($('#thisPage').val() == 'document') {/*用来配置开放式搜索运营商书架页面*/
                if (pathName.indexOf('bookshelf') > -1) {
                    _this.changeLang(a, 'bookshelf');
                } else if (pathName.indexOf('technology') > -1) {
                    _this.changeLang(a, 'technology');
                } else if (pathName.indexOf('maintenance') > -1) {
                    _this.changeLang(a, 'maintenance');
                } else if (pathName.indexOf('engineer') > -1) {
                    _this.changeLang(a, 'engineer');
                } else if (pathName.indexOf('multimedia') > -1) {
                    _this.changeLang(a, 'multimedia');
                }
            } else if (pathName.indexOf('recomproduct') > -1) {
                _this.changeLang(a, 'recomproduct');
            } else if ($('#thisPage').val() == 'solution') {/*用来配置解决方案运营商书架页面*/
                _this.changeLang(a, 'solution');
            } else if ($('#thisPage').val() == 'UXtools') {
                _this.changeLang(a, 'tool');
            } else {
                _this.changeLang(a);
            }/* 将选择的值存储在缓存中*/
            sessionStorage.setItem("langName", textVal);
            sessionStorage.setItem("langNameVal", a);
            $('#headerTop').find('.lang-val').hide();
        }
    });
    $('#iphone-nav').find('.new-change-lang').off('click').on('click', function (e) {
        stopBubble(e);
        var str = $(this).text();
        var a = $(this).attr("lang");
        var pathName = window.location.pathname;
        var getEnDataFromGraph = $('#getEnDataFromGraph').val();
        if ((pathName.indexOf("enterprisesearch") >= 0 || pathName.indexOf("carriersearch") >= 0) && a != 'zh' && getEnDataFromGraph == 'N') {
            var keyword = $('#search-inputbox').val();
            var protocol = window.location.protocol;
            var hostName = window.location.host;
            var rootName = $("#common-chang-path").val();
            var domainStr = $('#domainStr').val();
            var uniqueUrl = $("#uniqueUrl").val();
            var lang = 'en';
            var newHref = protocol + "//" + hostName + rootName + "/" + lang + '/' + domainStr + '/index';/* 跳转*/
            window.location.href = newHref;
        } else {
            var domainStr = $('#domainStr').val();
            if (pathName.indexOf('encyclopedia') > -1) {
                _this.changeLang(a, 'encyclopedia');
            } else if (pathName.indexOf('search-center') > -1) {
                _this.changeLang(a, 'search-center');
            } else if (pathName.indexOf('enterprisesearch') > -1) {
                _this.changeLang(a, 'enterprisesearch');
            } else if ($('#thisPage').val() == 'document') {/*暂时用来配置开放式搜索运营商的其他需要传参的页面，等后来配置完标准网址后再做适配*/
                if (pathName.indexOf('bookshelf') > -1) {
                    _this.changeLang(a, 'bookshelf');
                } else if (pathName.indexOf('technology') > -1) {
                    _this.changeLang(a, 'technology');
                } else if (pathName.indexOf('maintenance') > -1) {
                    _this.changeLang(a, 'maintenance');
                } else if (pathName.indexOf('engineer') > -1) {
                    _this.changeLang(a, 'engineer');
                } else if (pathName.indexOf('multimedia') > -1) {
                    _this.changeLang(a, 'multimedia');
                }
            } else if (pathName.indexOf('recomproduct') > -1) {
                _this.changeLang(a, 'recomproduct');
            } else if ($('#thisPage').val() == 'solution') {/*用来配置解决方案运营商书架页面*/
                _this.changeLang(a, 'solution');
            } else {
                _this.changeLang(a);
            }/* 将选择的值存储在缓存中*/
            sessionStorage.setItem("langName", str);
            sessionStorage.setItem("langNameVal", a);
        }
    });
    $('#headerDIV').find('.header-mid .nav').off('click').on('click', function (e) {
        stopBubble(e);
        if ($('#thisPage').val() === 'solution') {
            return;
        }
        var href = $(this).attr('href');
        if ($(this).attr('target') != '_blank') {
            e.preventDefault();
            $(this).siblings().removeClass('active');
            $(this).addClass('active');
            window.location = href;
        }
    });/*运营商头部切换中英文*/
    $('#headerDIV').find('.carrier').find('.new-change-lang').off('click').on('click', function (e) {
        stopBubble(e);
        var textVal = $(this).text();
        var a = $(this).attr("lang");
        var pathName = window.location.pathname;
        var getEnDataFromGraph = $('#getEnDataFromGraph').val();
        if ((pathName.indexOf("enterprisesearch") >= 0 || pathName.indexOf("carriersearch") >= 0) && a != 'zh' && getEnDataFromGraph == 'N') {
            var keyword = $('#search-inputbox').val();
            var protocol = window.location.protocol;
            var hostName = window.location.host;
            var rootName = $("#common-chang-path").val();
            var domainStr = $('#domainStr').val();
            var uniqueUrl = $("#uniqueUrl").val();
            var lang = 'en';
            var newHref = protocol + "//" + hostName + rootName + "/" + lang + '/' + domainStr + '/index';/* 跳转*/
            window.location.href = newHref;
        } else {
            if (pathName.indexOf('encyclopedia') > -1) {
                _this.changeLang(a, 'encyclopedia');
            } else if (pathName.indexOf('search-center') > -1) {
                _this.changeLang(a, 'search-center');
            } else if (pathName.indexOf('carriersearch') > -1) {
                _this.changeLang(a, 'carriersearch');
            } else {/* 将选择的值存储在缓存中*/
                sessionStorage.setItem("langName", textVal);
                sessionStorage.setItem("langNameVal", a);
                $('#headerTop').find('.lang-val').hide();
            }
        }
    })
};/*得到用户的权限*/
function getUserLever() {
    var userLevelName = null;
    var isTest = $('#isTest').val();
    if (isTest != 'uat') {
        return "";
    }
    var userLevel = commonHead.userLevel;
    switch (userLevel) {
        case "1":
            userLevelName = $('#userName1').val();
            break;
        case "2":
            userLevelName = $('#userName2').val();
            break;
        case "3":
            userLevelName = $('#userName3').val();
            break;
        case "4":
            userLevelName = $('#userName4').val();
            break;
        case "5":
            userLevelName = $('#userName5').val();
            break;
        case "6":
            userLevelName = $('#userName6').val();
            break;
        default:
            userLevelName = "unKnow";
            break;
    }
    return "(" + userLevelName + ")";
}

var eHeadObject = new EHead();
$(function () {
    $.when(eHeadObject.init()).done(function (a) {
        eHeadObject.getUserState();
        if (!isOpenSearchMobile()) {
            changeRoleLevel(eHeadObject.isLogin, eHeadObject.state, eHeadObject.USER_ID);
        }
        eHeadObject.initListener()
    });
    if ($(".lazyload img").length > 0) {
        $(".lazyload img").lazyload({
            threshold: 100,
            failure_limit: 100,
            event: "scroll",
            effect: "fadeIn",
            container: window,
            data_attribute: "original",
            skip_invisible: true,
            appear: function () {
                var b = $(this).parents(".lazyload");
                if (b.parents(".ebg-bg").length && b.parents(".ebg-bg").data("replace") != "not") {
                    b.parents(".ebg-bg").css("background-image", "url(" + $(this).data("original") + ")");
                    $(this).data("original", "");
                    b.hide()
                }
            },
            load: function () {
                var b = $(this).parents(".lazyload");
                if (b.parents(".ebg-bg").length && b.parents(".ebg-bg").data("replace") != "not") {
                } else {
                    b.css({height: "auto", background: "none"}).addClass("over")
                }
            }
        })
    }
    if ($(".lazyload-v2 img").length > 0) {
        $(".lazyload-v2 img").lazyload({
            threshold: 100,
            failure_limit: 100,
            event: "scroll",
            effect: "fadeIn",
            container: window,
            data_attribute: "original",
            skip_invisible: true,
            appear: null,
            load: function () {
                $(this).parents(".lazyload-v2").css({height: "auto", background: "none"}).addClass("over")
            }
        })
    }
});
window.Base = window.Base || function () {
};
window.console = window.console || {
    log: function () {
    }, warn: function () {
    }, error: function () {
    }, info: function () {
    }
};
jQuery.cookie = function (b, j, m) {
    if (typeof j != "undefined") {
        m = m || {};
        if (j === null) {
            j = "";
            m = $.extend({}, m);
            m.expires = -1
        }
        var e = "";
        if (m.expires && (typeof m.expires == "number" || m.expires.toUTCString)) {
            var f;
            if (typeof m.expires == "number") {
                f = new Date();
                f.setTime(f.getTime() + (m.expires * 24 * 60 * 60 * 1000))
            } else {
                f = m.expires
            }
            e = "; expires=" + f.toUTCString()
        }
        var l = m.path ? "; path=" + (m.path) : "";
        var g = m.domain ? "; domain=" + (m.domain) : "";
        var a = m.secure ? "; secure" : "";
        document.cookie = [b, "=", encodeURIComponent(j), e, l, g, a].join("")
    } else {
        var d = null;
        if (document.cookie && document.cookie != "") {
            var k = document.cookie.split(";");
            for (var h = 0; h < k.length; h++) {
                var c = jQuery.trim(k[h]);
                if (c.substring(0, b.length + 1) == (b + "=")) {
                    d = decodeURIComponent(c.substring(b.length + 1));
                    break
                }
            }
        }
        return d
    }
};
(function (a) {
    a.fn.addBack = a.fn.addBack || a.fn.andSelf;
    a.fn.extend({
        actual: function (b, l) {
            if (!this[b]) {
                throw '$.actual => The jQuery method "' + b + '" you called does not exist'
            }
            var f = {absolute: false, clone: false, includeMargin: false};
            var i = a.extend(f, l);
            var e = this.eq(0);
            var h, j;
            if (i.clone === true) {
                h = function () {
                    var m = "position: absolute !important; top: -1000 !important; ";
                    e = e.clone().attr("style", m).appendTo(checkXssHTML("body"))
                };
                j = function () {
                    e.remove()
                }
            } else {
                var g = [];
                var d = "";
                var c;
                h = function () {
                    c = e.parents().addBack().filter(":hidden");
                    d += "visibility: hidden !important; display: block !important; ";
                    if (i.absolute === true) {
                        d += "position: absolute !important; "
                    }
                    c.each(function () {
                        var m = a(this);
                        g.push(m.attr("style"));
                        m.attr("style", d)
                    })
                };
                j = function () {
                    c.each(function (m) {
                        var o = a(this);
                        var n = g[m];
                        if (n === undefined) {
                            o.removeAttr("style")
                        } else {
                            o.attr("style", n)
                        }
                    })
                }
            }
            h();
            var k = /(outer)/.test(b) ? e[b](i.includeMargin) : e[b]();
            j();
            return k
        }
    })
})(jQuery);
Base.prototype.contourFun = function (c, b, e, a) {
    var f = $(c), d = $(window).width();
    if (f.length > 0) {
        if (a == undefined || d > a) {
            $.each(f, function (j, n) {
                n = $(n);
                var g = n.find(b), s = g.find(e), h = n.actual("outerWidth"), r = g.actual("width") - 1,
                    q = Math.floor(h / r), k = [], l;
                if (h > 0 && q > 0) {
                    var p = Math.ceil(g.length / q);
                    s.height("auto");
                    for (var m = 0; m < p; m++) {
                        k = [];
                        for (var o = q * m; o < q * (m + 1); o++) {
                            k.push(s.eq(o).actual("height"))
                        }
                        l = Math.max.apply(null, k);
                        for (var o = q * m; o < q * (m + 1); o++) {
                            s.eq(o).height(l)
                        }
                    }
                }
            })
        } else {
            f.find(b).find(e).height("auto")
        }
    }
};
Base.prototype.promptIncident = function () {
    $(".prompt-popup").on("click", function () {
        var b = "." + $(this).data("vaule");
        (b && b == ".map-reveal-module") && $(b).addClass("map-reveal-module-bg");
        var a = $(document).scrollTop();
        $("body").addClass("position-fix").css("margin-top", -a);
        $(b).fadeIn("fast").css("overflow-y", "auto")
    });
    $(".prompt-clock").on("click", function (b) {
        var c = b || window.event;
        c.stopPropagation();
        var a = Math.abs(parseInt($("body").css("margin-top")));
        $("body").removeClass("position-fix").css("margin-top", 0);
        $("html,body").scrollTop(a);
        $(this).parents(".prompt-box").fadeOut("fast")
    })
};
Base.prototype.v2PromptIncident = function () {
    $(".v2-prompt-popup").on("click", function () {
        var b = "." + $(this).data("vaule");
        var a = $(document).scrollTop();
        $("body").addClass("position-fix").css("margin-top", -a);
        $(b).fadeIn("fast").css("overflow-y", "auto")
    });
    $(".v2-prompt-clock").on("click", function (b) {
        var c = b || window.event;
        c.stopPropagation();
        var a = Math.abs(parseInt($("body").css("margin-top")));
        $("body").removeClass("position-fix").css("margin-top", 0);
        $("html,body").scrollTop(a);
        $(this).parents(".v2-prompt-box").fadeOut("fast")
    })
};
Base.prototype.navigationIncident = function () {
    var g = 0, b = 0, e = 0, c = 0, f = $(window).width();
    var d = "";
    $(".nav-subset-module").each(function () {
        g = $(this).actual("width");
        b = $(this).parents(".nav-list").outerWidth();
        e = $(this).parents(".nav-list").offset().left;
        c = f - b - e;
        if (e < (g - b) / 2) {
            $(this).css("margin-left", -e)
        } else {
            if (c < (g - b) / 2) {
                $(this).css("margin-left", -(g - b - c + 1))
            } else {
                $(this).css("margin-left", (b - g) / 2)
            }
        }
    });
    $(".login").on("hover", function () {
        if ($(".logined-tip").length > 0) {
            $(".logined-tip").hide()
        }
    });
    $("body").on("click", function () {
        if ($(".logined-tip").length > 0) {
            $(".logined-tip").hide()
        }
    });
    $(".nav-quick").on("hover", function () {
        if ($(".logined-tip").length > 0) {
            $(".logined-tip").hide()
        }
    });
    $(".logined-tip").on("click", function (h) {
        h.stopPropagation()
    });
    $(".nav-section .nav-list").on("mouseleave", function () {
        if ($(".nav-section .nav-searchbit-text").length > 0) {
            $(".nav-section .nav-searchbit-text").blur()
        }
    });
    $(".nav-subset-height").each(function () {
        var j = 0;
        for (var h = 0; h < $(this).find(".nav-tabel-cell").length; h++) {
            var k = $(this).find(".nav-tabel-cell").eq(h).find(".nav-subset-box").eq(0);
            if (h == 0) {
                j = k.actual("outerHeight")
            } else {
                if (j < k.actual("outerHeight")) {
                    j = k.actual("outerHeight")
                }
            }
        }
        for (var h = 0; h < $(this).find(".nav-tabel-cell").length; h++) {
            $(this).find(".nav-tabel-cell").eq(h).find(".nav-subset-box").eq(0).outerHeight(j)
        }
    });
    $(".nav-subset-module").removeClass("nav-subset-height");

    function a() {
        if ($(".nav-section").length > 0) {
            if ($(window).scrollTop() < $(".nav-section").offset().top) {
                $(".nav-section .nav").css("position", "relative")
            } else {
                $(".nav-section .nav").css("position", "fixed")
            }
        }
    }

    a();
    $(window).on("scroll", function () {
        a()
    })
};
Base.prototype.BrowseHappyFun = function () {
    var a = "browsehappy";
    var b = $("#lang").val();
    if (b == "en" || b == "en-gb" || b == "fr" || b == "de" || b == "it" || b == "es") {
        a = "browsehappy_other"
    }
    if ($.cookie(a)) {
        return false
    } else {
        $(".browsehappy").slideDown(function () {
            $(this).trigger("slideEnd");
            $(window).trigger("scroll")
        })
    }
    $(".browsehappy a.close").on("click", function () {
        $(this).parents(".browsehappy").slideUp(function () {
            $(this).trigger("slideEnd");
            $(window).trigger("scroll");
            $.cookie(a, "browsehappy", {expires: 30, path: "/", domain: ""})
        })
    })
};
Base.prototype.searchIncident = function () {
    $(".search").on("click", function (a) {
        $(this).find(".search-box").addClass("on").find(".search-text").focus();
        $(document).on("click", function () {
            $(".search-box").removeClass("on")
        });
        a.stopPropagation()
    });
    $(".search-box").on("click", function (a) {
        a.stopPropagation()
    });
    $(".search .search-close").on("click", function (a) {
        $(this).parents(".search-box").removeClass("on");
        a.stopPropagation()
    });
    $(".mobile-search .mobile-search-a").on("click", function (a) {
        $(this).parents(".mobile-search").find(".mobile-search-box").slideToggle(100).find(".mobile-search-text").focus();
        $(document).on("click", function () {
            $(".mobile-search-box").slideUp(100)
        });
        a.stopPropagation()
    });
    $(".mobile-search-box").on("click", function (a) {
        a.stopPropagation()
    });
    $(".mobile-search-box span").on("click", function () {
        $(this).parents(".mobile-search-box").slideUp(100)
    })
};
Base.prototype.resizeStackLock = false;
Base.prototype.resizeStack = [];
Base.prototype.resizeChangeScreen = function () {
    var a;
    var e = this;
    var d = $(window).width();

    function c(g) {
        if (g <= 750) {
            return 0
        } else {
            if (g <= 1200) {
                return 1
            } else {
                return 2
            }
        }
    }

    function f() {
        return document.fullscreenElement || document.webkitFullscreenElement || document.mozFullScreenElement || document.msFullscreenElement
    }

    function b() {
        var g = f();
        return (g && $(g).attr("id") == "jWVideoFrameId") || $("#jWVideoFrameId").hasClass("vjs-playing") || $(".mudu-state-playing").length
    }

    $(window).on("resize", function () {
        clearTimeout(a);
        a = setTimeout(function () {
            var g = $(window).width();
            if (c(d) != c(g) && !b()) {
                if($("#hidden_o3").val() != 1){
                    location.reload()
                }
            } else {
                e.resizeStackLock = true;
                $.each(e.resizeStack, function (h, i) {
                    if (i && i.fn) {
                        i.fn.apply(this, i.params)
                    }
                });
                e.resizeStackLock = false
            }
        }, 400)
    })
};
Base.prototype.wapManuNav = function () {
    var a = $(window).height();
    $(".wap-navbox").height(a);
    $(".wap-nav").height(a);
    $("#wap-menubtn").on("click", function () {
        var b = $(document).scrollTop();
        $("body").addClass("position-fix").css("marginTop", -b);
        $("#wap-menu").fadeIn(100);
        $("#top").fadeOut()
    });
    $(".wap-menuclose").on("click", function () {
        if ($("#wap-menuprev").attr("data-prev") != "" && $("#wap-menuprev").attr("data-prev") != undefined) {
            $("#wap-menuprev").trigger("click")
        }
        var b = Math.abs(parseInt($("body").css("marginTop")));
        $("body").removeClass("position-fix").css("marginTop", 0);
        $("html,body").scrollTop(b);
        $("#wap-menu").fadeOut(100);
        $(".mob-nav-corporate").find('a[aria-expanded="true"]').trigger("click")
    });
    $("#wap-menuprev").on("click", function () {
        var d = $(this);
        var b = $("." + d.attr("data-prev"));
        if (d.attr("data-prev") == "opened1") {
            b.removeClass("wap-rollout opened1");
            $(".wap-navmain ul").removeClass("wap-putaway");
            $("#wap-menutitle").html("").removeClass("wap-show");
            $("#wap-menuprev").removeClass("wap-show").attr("data-prev", "");
            $(".wap-navbox").scrollTop(0)
        }
        if (d.attr("data-prev") == "opened2") {
            var c = $(".opened1").parents("li").find(".wap_firstcolumn span").text();
            b.removeClass("wap-rollout opened2");
            $("#wap-menutitle").text(c);
            $("#wap-menuprev").attr("data-prev", "opened1");
            $(".opened1").removeClass("wap-overflow-hide");
            $(".opened1").find("dl").first().removeClass("wap-putaway");
            $(".wap-navbox").scrollTop(0)
        }
    });
    $(".wap-navmain .wap_firstcolumn").on("click", function () {
        if ($(this).parents("li").find(".wap-navbox").length > 0) {
            var b = $(this).find("span").text();
            $("#wap-menutitle").text(b).addClass("wap-show");
            $("#wap-menuprev").addClass("wap-show").attr("data-prev", "opened1");
            $(".wap-navmain ul").addClass("wap-putaway");
            $(this).parents("li").find(".wap-navbox").first().addClass("wap-rollout opened1")
        }
    });
    $(".wap-navmain .wap_secondcolumn").on("click", function () {
        var b = $(this).find("span").text();
        $("#wap-menutitle").text(b).addClass("wap-show");
        $("#wap-menuprev").addClass("wap-show").attr("data-prev", "opened2");
        $(".wap-navbox").scrollTop(0);
        $(".opened1").addClass("wap-overflow-hide");
        $(".opened1").find("dl").first().addClass("wap-putaway");
        $(this).parents("dd").find(".wap-navbox").first().addClass("wap-rollout opened2")
    })
};
Base.prototype.replaceImg = function () {
    if ($(".replaceimg").length > 0) {
        var a = $(window).width();
        if (a > 768) {
            $(".replaceimg").each(function (b, c) {
                var d = $(this).data("pcsrc");
                $(this).attr("src", d)
            })
        } else {
            $(".replaceimg").each(function (b, c) {
                var d = $(this).data("wapsrc");
                $(this).attr("src", d)
            })
        }
    }
};
Base.prototype.replaceImglazyload = function () {
    if ($(".replaceimglazyload").length > 0) {
        var c = $(window).width();
        var a = $(".replaceimglazyload");
        if (c > 768) {
            var d = a.data("pcsrc");
            a.attr("data-original", d)
        } else {
            var b = a.data("wapsrc");
            a.attr("data-original", b)
        }
    }
};
Base.prototype.ebgReplaceImgLazyLoad = function () {
    function a() {
        if ($(".J-ebg-replace-img-lazyload").length > 0) {
            var b = $(window).width();
            $(".J-ebg-replace-img-lazyload").each(function () {
                var g = $(this);
                var h = g.attr("src");
                var i = g.data("pc-l");
                var f = g.data("pc-s");
                var d = g.data("ipad-h");
                var c = g.data("ipad-v");
                var e = g.data("wap");
                if (b > 1600) {
                    g.attr("data-original", i || f || d || c || e || h)
                } else {
                    if (b > 1200) {
                        g.attr("data-original", f || i || d || c || e || h)
                    } else {
                        if (b > 992) {
                            g.attr("data-original", d || f || i || c || e || h)
                        } else {
                            if (b > 750) {
                                g.attr("data-original", c || e || d || f || i || h)
                            } else {
                                g.attr("data-original", e || c || d || f || i || h)
                            }
                        }
                    }
                }
            })
        }
    }

    $("body").on("ebgReplaceImgLazyLoad", function () {
        a()
    });
    a()
};
Base.prototype.ebgReplaceImg = function () {
    function a() {
        if ($(".J-ebg-replace-img").length > 0) {
            var b = $(window).width();
            $(".J-ebg-replace-img").each(function () {
                var g = $(this);
                var h = g.attr("src");
                var i = g.data("pc-l");
                var f = g.data("pc-s");
                var d = g.data("ipad-h");
                var c = g.data("ipad-v");
                var e = g.data("wap");
                if (b > 1600) {
                    g.attr("src", i || f || d || c || e || h)
                } else {
                    if (b > 1200) {
                        g.attr("src", f || i || d || c || e || h)
                    } else {
                        if (b > 992) {
                            g.attr("src", d || f || i || c || e || h)
                        } else {
                            if (b > 750) {
                                g.attr("src", c || d || f || i || e || h)
                            } else {
                                g.attr("src", e || c || d || f || i || h)
                            }
                        }
                    }
                }
            })
        }
    }

    $("body").on("ebgReplaceImg", function () {
        a()
    });
    a()
};
Base.prototype.browserVersion = function () {
    var k = navigator.userAgent;
    if ((k.match(/(phone|pad|pod|iPhone|iPod|ios|iPad|Android|Mobile|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i))) {
        var c = !!k.match(/\(i[^;]+;( U;)? CPU.+Mac OS X/);
        var g = k.indexOf("Android") > -1 || k.indexOf("Adr") > -1;
        if (c) {
            return "ios"
        }
        if (g) {
            if (k.match(/HUAWEI/i) && k.match(/MQQBrowser/i)) {
                return "huawei"
            } else {
                if (k.match(/HONOR/i) && k.match(/MQQBrowser/i)) {
                    return "honor"
                } else {
                    return "android"
                }
            }
        }
    } else {
        var b = k.indexOf("Opera") > -1;
        var e = k.indexOf("compatible") > -1 && k.indexOf("MSIE") > -1 && !b;
        var l = k.indexOf("rv:11") > -1;
        var i = k.indexOf("Edge") > -1 && !e;
        var d = k.indexOf("Firefox") > -1;
        var h = k.indexOf("Safari") > -1 && k.indexOf("Chrome") == -1;
        var a = k.indexOf("Chrome") > -1 && k.indexOf("Safari") > -1 && k.indexOf("Edge") == -1;
        if (e) {
            var j = new RegExp("MSIE (\\d+\\.\\d+);");
            j.test(k);
            var f = parseFloat(RegExp["$1"]);
            switch (f) {
                case 6:
                    return "6";
                case 7:
                    return "7";
                case 8:
                    return "8";
                case 9:
                    return "9";
                case 10:
                    return "10";
                case 11:
                    return "11";
                default:
                    return "0"
            }
        }
        if (d) {
            return "FF"
        }
        if (b) {
            return "Opera"
        }
        if (h) {
            return "Safari"
        }
        if (a) {
            return "Chrome"
        }
        if (i) {
            return "Edge"
        }
        if (l) {
            return "11"
        }
    }
};
Base.prototype.v2SetHeight = function (c, b, a) {
    if (!this.resizeStackLock && this.resizeStack) {
        this.resizeStack.push({fn: this.v2SetHeight, params: arguments})
    }
    if (baseLib.browserVersion() < 11) {
        var d = $(c);
        d.each(function () {
            var q = $(this), k = q.find(b), o = k.length, f = q.outerWidth(true) + 10, h = k.outerWidth(true),
                m = Math.floor(f / h), e = Math.ceil(o / m), l = 0, g = null;
            k.find(a).height("auto");
            for (var p = 0; p < e; p++) {
                for (var n = p * m; n < m * (p + 1); n++) {
                    k.eq(n).addClass("oList_obj");
                    var r = k.eq(n).find(a).height();
                    r > l ? l = r : l;
                    g = $(".oList_obj")
                }
                g.find(a).height(l);
                g.removeClass("oList_obj");
                l = 0
            }
        })
    }
};
Base.prototype.lineLimit = function () {
    function a(g, j, b) {
        var i = g.text();
        if (g.data("org-text")) {
            i = g.data("org-text")
        } else {
            g.data("org-text", i)
        }
        j = j || 1;
        var d = $("#J-line-limit-text");
        if (d.length == 0) {
            $("body").append('<div id="J-line-limit-text" style="position:absolute;opacity:0;left:-100%;top: -100px;"></div>')
        }
        d = $("#J-line-limit-text").css({
            fontSize: g.css("font-size"),
            lineHeight: g.css("line-height"),
            width: g.width(),
            wordBreak: g.css("word-break"),
        });
        var h = d.html("...").height() * j;
        if (b) {
            i = i.split(/([^x00-xff]|\s+)/ig)
        }
        var e = i.length;
        var f = false;
        while (e--) {
            var c;
            if (i[e] == "") {
                continue
            }
            if (typeof i == "object") {
                c = i.slice(0, e + 1).join("")
            } else {
                c = i.slice(0, e + 1)
            }
            d.html(c + "...");
            if (d.height() > h) {
                f = true;
                continue
            }
            if (f) {
                return c.slice(0, c.length - 1) + "..."
            } else {
                return i
            }
        }
        return ""
    }

    $(document).off("limit", ".J-line-limit").on("limit", ".J-line-limit", function () {
        var g = $(window).width();
        var h = $(this);
        if (h.data("init-limit") || h.is(":hidden")) {
            return
        }
        h.data("init-limit", true);
        var b = h.data("limit");
        var i = h.data("limit-pc-s");
        var d = h.data("limit-ipad-h");
        var c = h.data("limit-ipad-s");
        var e = h.data("limit-wap");
        var f = false;
        if (i && g > 1600) {
            f = true
        }
        if (d && g > 1200) {
            f = true
        }
        if (c && g > 992) {
            f = true
        }
        if (e && g > 750) {
            f = true
        }
        if (!f && !(i || d || c || e)) {
            f = true
        }
        if (f) {
            h.html(a(h, b, h.data("limit-type")))
        }
    });
    $(".J-line-limit").trigger("limit");
    let ts = setTimeout(function () {
        clearTimeout(ts);
        $(".J-line-limit").data("init-limit", false);
        $(".J-line-limit").trigger("limit")
    }, 1000)
};
Base.prototype.v3SetHeight = function (d, c, b) {
    if (!this.resizeStackLock && this.resizeStack) {
        this.resizeStack.push({fn: this.v3SetHeight, params: arguments})
    }
    var e = $(d);
    var a = arguments;
    e.each(function () {
        var r = $(this), l = r.find(c), p = l.length, g = r.width() + 10, k = l.width(), n = Math.floor(g / k),
            f = Math.ceil(p / n), m = 0, h = null;
        if (a.length == 2) {
            l.height("auto")
        } else {
            l.find(b).height("auto")
        }
        for (var q = 0; q < f; q++) {
            for (var o = q * n; o < n * (q + 1); o++) {
                l.eq(o).addClass("oList_obj");
                if (a.length == 2) {
                    var s = l.eq(o).height()
                } else {
                    var s = l.eq(o).find(b).height()
                }
                s > m ? m = s : m;
                h = $(".oList_obj")
            }
            if (a.length == 2) {
                h.height(m)
            } else {
                h.find(b).height(m)
            }
            h.removeClass("oList_obj");
            m = 0
        }
    })
};
Base.prototype.v2FooterFun = function () {
    $(".foot-nav-cell").on("click", function () {
        var a = $(this);
        if (!a.hasClass("current")) {
            $(".foot-nav-cell").removeClass("current");
            $(".foot-nav-cell").find("dd").removeClass("current");
            $(".foot-nav-cell").find(".iconfont").addClass("icon-expansion").removeClass("icon-collapse");
            a.addClass("current");
            a.find("dd").addClass("current");
            a.find(".iconfont").removeClass("icon-expansion").addClass("icon-collapse")
        } else {
            a.removeClass("current");
            a.find("dd").removeClass("current");
            a.find(".iconfont").addClass("icon-expansion").removeClass("icon-collapse")
        }
    })
};
Base.prototype.globalToolbar = function () {
    var a = null;
    var c = $(".global_toolbar");
    var b = $(".toolbar_btn").height();
    if ($(".bannar-breadcrumbs-wrap").height() > 100) {
        a = $(".bannar-breadcrumbs-wrap").height() + $(".zl-pc-header").height() + 50;
        $(".toolbar_btn").css({top: a, marginTop: 0})
    } else {
        $(".global_toolbar").removeClass("default");
        $(".toolbar_btn").css({top: "50%", marginTop: -b / 2})
    }
    $(window).on("scroll.toolbar", function () {
        var d = $(window).scrollTop();
        var e = $(window).height();
        if (c.hasClass("default")) {
            if (d + e / 2 >= a + b / 2) {
                $(".global_toolbar").removeClass("default opacity");
                $(".toolbar_btn").removeClass("default").css({top: "50%", marginTop: -b / 2})
            }
        } else {
            if (d > 10) {
                $(".global_toolbar").removeClass("opacity");
                $(".toolbar_btn").removeClass("default")
            }
        }
    });
    $(".reading_container").on("scroll.readingcontainer", function () {
        $(".global_toolbar").removeClass("opacity");
        $(".toolbar_btn").removeClass("default");
        $(".reading_container").off("scroll.readingcontainer");
        $(".global_toolbar").removeClass("default")
    });
    $(".global_toolbar").on("mouseleave", ".toolbar_btn", function () {
        $(".global_toolbar").removeClass("opacity");
        $(".toolbar_btn").removeClass("default")
    })
};
Base.prototype.globalTopButton = function () {
    var c = $(window).width();
    if ($("div#top").length > 0 || $("a#top").length > 0) {
        $("#top").detach()
    }
    $("<div/>", {
        id: "global-top-button",
        "class": "iconfont icon-up",
        style: "display:none;box-shadow: rgb(238, 238, 238) 0px 0px 8px;;border-radius:2px;background:#fff;position:fixed;bottom:100px;right:35px;width:56px;height:56px;text-align:center;padding-top:10px;font-size:33px;color: #999;cursor:pointer;z-index:1001;line-height:1;"
    }).appendTo("body");
    var b = $("#global-top-button");
    var a = 0;
    if ($(window).width() < 750) {
        b.css({bottom: 120, right: 20})
    } else {
        if (750 < $(window).width() && $(window).width() < 1200) {
            b.css({bottom: 90, right: 35})
        } else {
            b.on({
                mouseenter: function () {
                    b.css("color", "#333")
                }, mouseleave: function () {
                    b.css("color", "#666")
                }
            })
        }
    }
    if (baseLib.browserVersion() == "huawei" && c < 750) {
        b.css("bottom", 120)
    }
    $(window).on("scroll", function () {
        var d = $(window).scrollTop();
        if (d > 700) {
            b.show()
        } else {
            b.hide()
        }
    });
    b.on("click", function () {
        $("body,html").animate({scrollTop: 0})
    })
};
var baseLib = new Base();
$(function () {
    baseLib.wapManuNav();
    baseLib.navigationIncident();
    baseLib.promptIncident();
    baseLib.searchIncident();
    baseLib.contourFun(".subset-industry-I", "dd", "span");
    baseLib.BrowseHappyFun();
    baseLib.resizeChangeScreen();
    baseLib.replaceImg();
    baseLib.replaceImglazyload();
    baseLib.ebgReplaceImgLazyLoad();
    baseLib.ebgReplaceImg();
    baseLib.browserVersion();
    baseLib.lineLimit();
    baseLib.v2FooterFun();
    baseLib.v2PromptIncident();
    baseLib.V2ReplaceImglazyload();
    baseLib.V2ReplaceImg();
    baseLib.globalToolbar();
    baseLib.globalTopButton()
});
var win_W = $(window).width();
$(window).on("resize", function () {
    var a = $(window).width();
    if (a != win_W) {
        win_W = a;
        baseLib.replaceImg();
        baseLib.replaceImglazyload()
    }
});
$.fn.extend({
    setWidthHeight: function (b) {
        var f = {scale: 4 / 3};
        var d = $.extend({}, f, b);
        var e = $(this);
        var a = e.width();
        var c = a / d.scale;
        e.css("height", c);
        $(window).on("resize", function () {
            e.each(function () {
                if (!$(this).hasClass("over")) {
                    a = e.width();
                    c = a * d.h / d.w;
                    e.css("height", c)
                }
            })
        })
    }
});
$(function () {
    if ($(".lazyload img").length > 0) {
        $(".lazyload img").lazyload({
            threshold: 100,
            failure_limit: 100,
            event: "scroll",
            effect: "fadeIn",
            container: window,
            data_attribute: "original",
            skip_invisible: true,
            appear: function () {
                var b = $(this).parents(".lazyload");
                if (b.parents(".ebg-bg").length && b.parents(".ebg-bg").data("replace") != "not") {
                    b.parents(".ebg-bg").css("background-image", "url(" + $(this).data("original") + ")");
                    $(this).data("original", "");
                    b.hide()
                }
            },
            load: function () {
                var b = $(this).parents(".lazyload");
                if (b.parents(".ebg-bg").length && b.parents(".ebg-bg").data("replace") != "not") {
                } else {
                    b.css({height: "auto", background: "none"}).addClass("over")
                }
            }
        })
    }
    if ($(".lazyload-v2 img").length > 0) {
        $(".lazyload-v2 img").lazyload({
            threshold: 100,
            failure_limit: 100,
            event: "scroll",
            effect: "fadeIn",
            container: window,
            data_attribute: "original",
            skip_invisible: true,
            appear: null,
            load: function () {
                $(this).parents(".lazyload-v2").css({height: "auto", background: "none"}).addClass("over")
            }
        })
    }
});
(function (c) {
    if (c("#need_help_desktop").length > 0) {
        var b = 0;
        c(window).on("scroll", function () {
            if (b == 0) {
                if (c(document).scrollTop() > 100 && c(window).width() > 767) {
                    c("#need_help_desktop").fadeIn(330);
                    b = 1
                }
            }
        });
        var a = c("#need_help_popup ul").height() + 40;
        c("#need_help_desktop").on("click", function (d) {
            d.stopPropagation()
        });
        c("#need_help_desktop .tab_help_open").on("click", function (f) {
            f.stopPropagation();
            if (c(window).width() <= 767) {
                c("#need_help_popup").css("top", "132px").show()
            } else {
                if (c(this).hasClass("close")) {
                    if (c("#gLanguageCurrent").val() == "ar-sa") {
                        c("#need_help_desktop").animate({left: -250}, 330)
                    } else {
                        c("#need_help_desktop").animate({right: -250}, 330)
                    }
                    var d = c("#need_help_desktop .tab_help_open img").height() + 16;
                    c("#need_help_desktop #need_help_popup,#need_help_desktop .tab_help_open").animate({height: d}, function () {
                        c("#need_help_desktop .tab_help_open").removeClass("close clickopen").addClass("clickclose")
                    })
                } else {
                    if (c("#gLanguageCurrent").val() == "ar-sa") {
                        c("#need_help_desktop").animate({left: 0})
                    } else {
                        c("#need_help_desktop").animate({right: 0})
                    }
                    c("#need_help_desktop #need_help_popup,#need_help_desktop .tab_help_open").animate({height: 330}, function () {
                        c("#need_help_desktop .tab_help_open").addClass("close clickopen").removeClass("clickclose")
                    })
                }
            }
        });
        c("#footer_nav_mobile_back_to_top").on("click", function () {
            window.scrollTo(0, 0)
        });
        c("body").on("click", function () {
            if (c("#gLanguageCurrent").val() == "ar-sa") {
                c("#need_help_desktop").animate({left: -250}, 330)
            } else {
                c("#need_help_desktop").animate({right: -250}, 330)
            }
            var d = c("#need_help_desktop .tab_help_open img").height() + 16;
            c("#need_help_desktop #need_help_popup,#need_help_desktop .tab_help_open").animate({height: d});
            c("#need_help_desktop .tab_help_open").removeClass("close clickopen").addClass("clickclose")
        })
    }
})(jQuery);
$(function () {
    var a = a || {};
    a.worldwide_language = function () {
        $("#worldwide-link-mob").empty().append($("#worldwide").children().clone());
        var b = function () {
            if (!(location.hash == "#worldwide")) {
                return
            }
            if ($(window).width() < 992) {
                $(".js-mobile-nav-open-btn").trigger("click");
                $("#worldwide-link-mob").collapse("show");
                return
            }
            $("#worldwide").collapse("show");
            $("html, body").animate({scrollTop: 0}, 500)
        };
        $(window).on("hashchange", function () {
            setTimeout(b, 50)
        });
        setTimeout(b, 800)
    };
    a.menu_click = function () {
        $(document).hoverIntent(function (c) {
            var b = $(this);
            b.addClass("active");
            let scollTimer = setTimeout(function () {
                clearTimeout(scollTimer);
                $(document).trigger("scroll")
            }, 200)
        }, function (c) {
            var b = $(this);
            b.removeClass("active")
        }, "#hw1_global_nav .nav-contact > ul > li");
        $(document).on("click.nav-gblnav", '.nav-gblnav [data-toggle="collapse"], #hw1_global_nav [data-toggle="collapse"]', function (b) {
            if (b.isTrigger == 3) {
                return
            }
            $(".nav-gblnav").find('a[data-toggle="collapse"]').filter('[aria-expanded="true"]').not(this).trigger("click")
        }).on("click.mob-nav-corporate", '.mob-nav-corporate [data-toggle="collapse"]', function (b) {
            if (b.isTrigger == 3) {
                return
            }
            $(this).closest(".mob-nav-corporate").find('a[data-toggle="collapse"]').filter('[aria-expanded="true"]').not(this).trigger("click")
        }).on("click.contact-us-meida", '.contact-us .tab-content .tab-pane [data-toggle="collapse"]', function (b) {
            if (b.isTrigger == 3) {
                return
            }
            $(this).closest(".tab-pane").find('a[data-toggle="collapse"]').filter('[aria-expanded="true"]').not(this).trigger("click")
        });
        $(document).on("click.nav-mobile", '[data-toggle="tab"], .nav-gblnav [data-toggle="collapse"], #hw1_global_nav [data-toggle="collapse"], #hw1_global_nav [data-toggle="tab"]', function (b) {
            let scollTimer = setTimeout(function () {
                clearTimeout(scollTimer);
                $(document).trigger("scroll")
            }, 200)
        });
        $(document).on("click.nav-mobile", ".js-mobile-nav-open-btn", function (c) {
            var b = $("body");
            var d = $(this);
            if (b.hasClass("mobile-menu-open")) {
                $(d.data("target")).removeClass("open");
                $("#container").removeClass("open");
                b.removeClass("mobile-menu-open");
                return false
            }
            $(d.data("target")).addClass("open");
            $("#container").addClass("open");
            b.addClass("mobile-menu-open");
            return false
        }).on("click.nav-mobile", ".js-mobile-nav-close-btn", function (b) {
            var c = $(this);
            $(c.data("target")).removeClass("open");
            $("#container").removeClass("open");
            $("body").removeClass("mobile-menu-open");
            return false
        })
    };
    a.menu_click()
});
var isadd = "0";
(function (a) {
    a.fn.extend({
        scrollBar: function (m) {
            var c = {barWidth: 5, position: "x,y", wheelDis: 15};
            m = a.extend(c, m);
            var i = a("html").hasClass("LANG-sa") ? "left" : "right";
            if (i == "left") {
                c.direction = 0
            }
            var b = '<div class="scrollBarBox" style="width:100%;' + i + ":0;height:" + c.barWidth + 'px;bottom:0;"><div class="scrollBar horizontalBar" style="height:' + c.barWidth + "px;border-radius:" + c.barWidth / 2 + 'px;"></div></div>';
            var j = '<div class="scrollBarBox" style="height:100%;top:0;width:' + c.barWidth + "px;" + i + ':0;"><div class="scrollBar verticalBar" style="width:' + c.barWidth + "px;border-radius:" + c.barWidth / 2 + 'px;"></div></div>';
            var d = this;
            d.each(function () {
                a(this).attr("paddingR", a(this).css("padding-right")).attr("paddingB", a(this).css("padding-bottom"))
            });

            function f(r) {
                var p = a(r).get(0);
                var v = parseFloat(a(r).attr("paddingR"));
                var q = parseFloat(a(r).attr("paddingB"));
                a(r).css({"padding-right": v + "px", "padding-bottom": q + "px", overflow: "hidden"});
                if (!(a(r).children().hasClass("scrollContentDiv"))) {
                    (baseLib.browserVersion() == "ios" || baseLib.browserVersion() == "android") ? a(r).wrapInner('<div class="scrollContentDiv oy-a"></div>') : a(r).wrapInner('<div class="scrollContentDiv"></div>')
                }
                if (a(r).css("position") == "static") {
                    a(r).css({position: "relative"})
                }
                var A = a(r).find(".scrollContentDiv");
                var t = A[0].scrollHeight;
                var y = A[0].scrollWidth;
                var o = A.height();
                var s = A.width();
                var w = a(r).innerHeight();
                var n = a(r).innerWidth();

                function u(C) {
                    if (baseLib.browserVersion() == 6) {
                        p.style.paddingRight = v + c.barWidth + "px"
                    } else {
                    }
                    o = a(C).height();
                    var D = w * (o / t);
                    a(C).find(".scrollBarBox").remove().end().append(j).find(".verticalBar").height(D)
                }

                function z(C) {
                    p.style.paddingBottom = q + c.barWidth + "px";
                    s = a(C).width();
                    var D = n * (s / y);
                    a(C).find(".scrollBarBox").remove().end().append(b).find(".horizontalBar").width(D)
                }

                switch (c.position) {
                    case "x,y":
                        if (t > o && y > s) {
                            p.style.paddingRight = v + c.barWidth + "px";
                            o = a(r).height();
                            w = a(r).innerHeight();
                            var x = (w - c.barWidth) * ((o - c.barWidth) / t);
                            if (!(a(r).find(".verticalBar").length)) {
                                a(r).append(j)
                            }
                            a(r).find(".verticalBar").height(x).parent().height(w - c.barWidth);
                            p.style.paddingBottom = q + c.barWidth + "px";
                            s = a(r).width();
                            var B = (n - c.barWidth) * ((s - c.barWidth) / y);
                            if (!(a(r).find(".horizontalBar").length)) {
                                a(r).append(b)
                            }
                            a(r).find(".horizontalBar").width(B).parent().width(n).css({
                                "padding-right": c.barWidth + "px",
                                "box-sizing": "border-box"
                            })
                        } else {
                            if (t > o) {
                                u(r)
                            } else {
                                if (y > s) {
                                    z(r)
                                } else {
                                    a(r).find(".scrollBarBox").remove()
                                }
                            }
                        }
                        break;
                    case "x":
                        if (y > s) {
                            z(r)
                        } else {
                            a(r).find(".scrollBarBox").remove()
                        }
                        break;
                    case "y":
                        if (t > o) {
                            u(r)
                        } else {
                            a(r).find(".scrollBarBox").remove()
                        }
                        break
                }
            }

            function l() {
                d.each(function () {
                    f(this)
                })
            }

            l();

            function k(p, o, s, n, q, r) {
                r = setInterval(function () {
                    var A = null;
                    var t = null;
                    var z = null;
                    var E = null;
                    var G = null;
                    var D = null;
                    var y = null;
                    var w = null;
                    var H = a(p).find(".scrollContentDiv");
                    if (H.length) {
                        A = H[0].scrollWidth;
                        t = H[0].scrollHeight;
                        z = H.width();
                        E = H.height()
                    } else {
                        A = a(p)[0].scrollWidth - parseFloat(a(p).css("padding-left"));
                        t = a(p)[0].scrollHeight - parseFloat(a(p).css("padding-top"));
                        z = a(p).width();
                        E = a(p).height()
                    }
                    if (A != o || t != s || z != n || E != q) {
                        if (H.length) {
                            G = parseFloat(H.css("top"));
                            D = parseFloat(H.css("left"))
                        }
                        if (a(p).find(".verticalBar").length) {
                            y = parseFloat(a(p).find(".verticalBar").css("top"))
                        }
                        if (a(p).find(".horizontalBar").length) {
                            w = parseFloat(a(p).find(".horizontalBar").css("left"))
                        }
                        f(p);
                        if (a(p).find(".scrollBarBox").length) {
                            if (G) {
                                var v = H[0].scrollHeight - a(p).height();
                                var F = H[0].scrollWidth - a(p).width();
                                if (-D > F) {
                                    D = -F
                                }
                                if (-G > v) {
                                    G = -v
                                }
                                H.css({left: D + "px", top: G + "px"})
                            }
                            if (y && a(p).find(".verticalBar").length) {
                                var x = a(p).find(".verticalBar");
                                var B = x.parent().height() - x.height();
                                if (y > B) {
                                    y = B
                                }
                                x.css("top", y + "px")
                            }
                            if (w && a(p).find(".horizontalBar").length) {
                                var C = a(p).find(".verticalBar");
                                var u = C.parent().width() - C.width();
                                if (w > u) {
                                    w = u
                                }
                                a(p).find(".horizontalBar").css("left", w + "px")
                            }
                        }
                        o = A;
                        s = t;
                        q = E;
                        n = z
                    }
                }, 100)
            }

            function e() {
                a.each(d, function (q, p) {
                    var r = p;
                    var u = "timer" + q;
                    var o = null;
                    var t = null;
                    var n = null;
                    var s = null;
                    if (a(p).find(".scrollContentDiv").length) {
                        o = a(p).find(".scrollContentDiv")[0].scrollWidth;
                        t = a(p).find(".scrollContentDiv")[0].scrollHeight;
                        n = a(p).find(".scrollContentDiv").width();
                        s = a(p).find(".scrollContentDiv").height()
                    } else {
                        o = a(r)[0].scrollWidth - parseFloat(a(r).css("padding-left"));
                        t = a(r)[0].scrollHeight - parseFloat(a(r).css("padding-top"));
                        n = a(r).width();
                        s = a(r).height()
                    }
                    k(r, o, t, n, s, u)
                })
            }

            e();

            function h() {
                a.each(d, function (n, o) {
                    var p = "timer" + n;
                    clearInterval(p)
                })
            }

            this.on("mousedown", ".scrollBar", function (u) {
                h();
                var w = null;
                if (a(this).hasClass("verticalBar")) {
                    w = "0"
                } else {
                    if (a(this).hasClass("horizontalBar")) {
                        w = "1"
                    }
                }
                var q = a(this).get(0);
                var A = a(this).parent().height() - a(this).height();
                var o = a(this).parent().width() - a(this).width();
                var r = a(this).parent().parent().find(".scrollContentDiv").get(0);
                var t = r.scrollHeight;
                var p = a(this).parent().parent().height();
                var z = r.scrollWidth;
                var s = a(this).parent().parent().width();
                var u = u || event;
                var x = u.clientY - q.offsetTop;
                var y = u.clientX - q.offsetLeft;
                var n = null;
                var v = null;
                switch (w) {
                    case "0":
                        document.onmousemove = function (B) {
                            var B = B || event;
                            if (B.clientY - x <= 0) {
                                n = 0
                            } else {
                                if ((B.clientY - x) >= A) {
                                    n = A
                                } else {
                                    n = B.clientY - x
                                }
                            }
                            q.style.top = n + "px";
                            r.style.top = -(n * (t - p) / A) + "px"
                        };
                        break;
                    case "1":
                        document.onmousemove = function (B) {
                            var B = B || event;
                            if (B.clientX - y <= 0) {
                                v = 0
                            } else {
                                if ((B.clientX - y) >= o) {
                                    v = o
                                } else {
                                    v = B.clientX - y
                                }
                            }
                            q.style.left = v + "px";
                            r.style.left = -(v * (z - s) / o) + "px"
                        };
                        break
                }
                document.onmouseup = function () {
                    document.onmousemove = null;
                    document.onmouseup = null;
                    e()
                };
                return false
            });

            function g(u, q) {
                if (q.find(".verticalBar").length) {
                    var p = true;
                    var o = q.find(".scrollBarBox").height() - q.find(".scrollBar").height();
                    var t = q.find(".scrollContentDiv").get(0);
                    var r = t.scrollHeight;
                    var n = q.height();
                    if (u.wheelDelta) {
                        p = u.wheelDelta > 0 ? true : false
                    } else {
                        p = u.detail < 0 ? true : false
                    }
                    var s = parseFloat(q.find(".scrollBar").css("top"));
                    if (p) {
                        s -= c.wheelDis;
                        if (s < 0) {
                            s = 0
                        }
                    } else {
                        s += c.wheelDis;
                        if (s > o) {
                            s = o
                        }
                    }
                    q.find(".scrollBar").get(0).style.top = s + "px";
                    q.find(".scrollContentDiv").get(0).style.top = -(s * (r - n) / o) + "px"
                }
            }

            d.each(function () {
                var n = a(this).get(0);
                if (n.addEventListener) {
                    n.addEventListener("DOMMouseScroll", function (p) {
                        var p = p || event;
                        var o = a(this);
                        g(p, o);
                        p.preventDefault()
                    }, false)
                }
                n.onmousewheel = function (p) {
                    if (baseLib.browserVersion() == "ios" || baseLib.browserVersion() == "android") {
                        return false
                    }
                    var p = p || event;
                    var o = a(this);
                    g(p, o);
                    return false
                }
            });
            return this
        }
    })
})(jQuery);
$(function () {
    function a() {
        var b = $(".ebg-footer");
        b.find(".link-set>ul").each(function () {
            var d = $(this).find(">li").eq(0);
            var e = $(this).find(">li:gt(0)");
            var c = $(window).width();
            if (c > 920) {
                d.off("click");
                return
            }
            d.off("click").on("click", function () {
                var f = $(window).width();
                if (f > 920) {
                    return
                }
                $(this).parent().toggleClass("active");
                $(this).toggleClass("active");
                e.toggleClass("active")
            })
        })
    }

    window.baseLib && baseLib.resizeStack.push({
        fn: function () {
            a()
        }
    });
    a()
});
var flag = false;
var hasKeyword = false;
var lastCompleteAjax = null;
$(document).ready(function () {
    initSearchSubPage();
});

function initSearchSubPage() {
    $(".support_input_focus").each(function () {
        $(this).val($(this).next(".inputInfohint").val())
    });
    $(".support_input_focus").on("focus", function () {
        searchClear($(this))
    });
    $(".support_input_focus").on("blur", function () {
        searchShow($(this))
    });
    $(".Find_support").on("click", function () {
        var b = $(this).parents(".productSupportDivNew").data("type");
        var a = $(this).parents(".productSupportDivNew").find(".support_input_focus").val();
        var c = $(this).parents(".productSupportDivNew").find(".Find_Language").val();
        if (c == undefined || c == "") {
            c = "en"
        }
        if (b == "all") {
        } else {
            const newH = window.open("/enterprisesearch?lang=" + c + "#type=searchAll&keyword=" + encodeURIComponent(enterpriseHTMLDecode(a)));
            newH.opener = null;
        }
    })
}

function EnterSupportSearch(b) {
    var a = window.event || arguments.callee.caller.arguments[0];
    if (a.keyCode == 13) {
        $(b).parents(".productSupportDivNew").find(".Find_support").trigger("click")
    }
}

function searchClear(c) {
    var a = $(c).parents(".productSupportDivNew").find(".inputInfohint").val();
    var b = $(c).parents(".productSupportDivNew").find(".support_input_focus");
    if (b.val() == a) {
        b.val("");
        b.css("color", "#000")
    }
}

function searchShow(c) {
    var a = $(c).parents(".productSupportDivNew").find(".inputInfohint").val();
    var b = $(c).parents(".productSupportDivNew").find(".support_input_focus");
    if (b.val().length <= 0) {
        b.val(a);
        b.css("color", "#8e8e8e")
    }
}

function loadHeadScript(c, d) {
    var b = document.createElement("script");
    b.type = "text/javascript";
    b.src = c;
    if (typeof d == "function") {
        b.addEventListener("load", d)
    }
    var a = document.getElementsByTagName("head")[0];
    a.appendChild(b)
}

$(document).ready(function () {
    showServiceSelfLinks()
});

function showServiceSelfLinks() {
    var c = $("#need_help_desktop_new");
    var a = null;
    var b = $(".irobotbar_btn").height();
    $(window).on("scroll.toolbar", function () {
        var d = $(window).scrollTop();
        if (c.hasClass("default")) {
            if (d + winH / 2 >= a + b / 2) {
                $(".irobotbar_btn").removeClass("default").css({top: "50%", marginTop: -b / 2})
            }
        } else {
            if (d > 10) {
                $(".irobotbar_btn").removeClass("default")
            }
        }
    });
    $(".reading_container").on("scroll.readingcontainer", function () {
        $(".irobotbar_btn").removeClass("default");
        $(".reading_container").off("scroll.readingcontainer")
    });
    $("#need_help_desktop_new").on("mouseleave", ".irobotbar_btn", function () {
        $(".irobotbar_btn").removeClass("default")
    })
};Base.prototype.V2ReplaceImglazyload = function () {
    if ($(".J-V2Replaceimglazyload").length > 0) {
        var a = $(window).width();
        $(".J-V2Replaceimglazyload").each(function () {
            var b = $(this);
            if (b.data("ipadsrc") != undefined) {
                if (a > 1200) {
                    var e = b.data("pcsrc");
                    b.attr("data-original", e)
                } else {
                    if (1200 > a && a > 750) {
                        var c = b.data("ipadsrc");
                        b.attr("data-original", c)
                    } else {
                        var d = b.data("wapsrc");
                        b.attr("data-original", d)
                    }
                }
            } else {
                if (a > 750) {
                    var e = b.data("pcsrc");
                    b.attr("data-original", e)
                } else {
                    var d = b.data("wapsrc");
                    b.attr("data-original", d)
                }
            }
        })
    }
};
Base.prototype.V2ReplaceImg = function () {
    if ($(".J-V2ReplaceImg").length > 0) {
        var a = $(window).width();
        $(".J-V2ReplaceImg").each(function () {
            var b = $(this);
            if (b.data("ipadsrc") != undefined) {
                if (a > 1200) {
                    var e = b.data("pcsrc");
                    if (b.is("img")) {
                        b.attr("src", e)
                    } else {
                        b.css("backgroundImage", "url(" + e + ")")
                    }
                } else {
                    if (1200 > a && a > 750) {
                        var c = b.data("ipadsrc");
                        if (b.is("img")) {
                            b.attr("src", c)
                        } else {
                            b.css("backgroundImage", "url(" + c + ")")
                        }
                    } else {
                        var d = b.data("wapsrc");
                        if (b.is("img")) {
                            b.attr("src", d)
                        } else {
                            b.css("backgroundImage", "url(" + d + ")")
                        }
                    }
                }
            } else {
                if (a > 750) {
                    var e = b.data("pcsrc");
                    if (b.is("img")) {
                        b.attr("src", e)
                    } else {
                        b.css("backgroundImage", "url(" + e + ")")
                    }
                } else {
                    var d = b.data("wapsrc");
                    if (b.is("img")) {
                        b.attr("src", d)
                    } else {
                        b.css("backgroundImage", "url(" + d + ")")
                    }
                }
            }
        })
    }
};

function goToPrivacy() {
    const lang = $('#language').val();
    const domainStr = $('#domainStr').val();
    const url = $('#common-chang-path').val() + '/vue/' + lang + '/' + domainStr + '/privacy/querySign';
    localStorage.setItem('beforePath', location.href);
    localStorage.setItem('isFromS', '1');
    const newH = window.open(checkXssHTML(url));
    newH.opener = null;
}

function infofinderLogout() {
    $.ajax({
        contentType: 'application/json;charset=utf-8',
        type: "get",
        url: $('#common-chang-path').val() + '/infofinderLogout',
        dataType: "json",
        async: false,
    });
}

function queryPrivacyCheck(versionNo) {
    let isC = '';
    $.ajax({
        contentType: 'application/json;charset=utf-8',
        type: "get",
        url: $('#common-chang-path').val() + '/privacy/queryPrivacyCheck?versionNo=' + versionNo + '&domain=' + $('#domain').val(),
        dataType: "json",
        async: false,
        success: function (res) {
            isC = res.data;
        },
        error: function () {
        }
    });
    return isC;
}

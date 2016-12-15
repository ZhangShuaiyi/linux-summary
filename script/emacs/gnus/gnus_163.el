(setq gnus-select-method '(nntp "news.newsfan.net"))

(setq user-mail-address "zhang_syi@163.com"
      user-full-name "Zhang Shuaiyi")

(add-to-list 'gnus-secondary-select-methods '(nnml ""))
(setq mail-sources '((pop :server "pop.163.com"
                          :user "zhang_syi@163.com"
                          :password {password}
                          :leave t)))

(setq send-mail-function            'smtpmail-send-it
      message-send-mail-function    'smtpmail-send-it
      smtpmail-smtp-service 25
      smtpmail-smtp-server          "smtp.163.com")

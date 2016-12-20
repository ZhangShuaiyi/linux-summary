(require 'nnir)

;; @see http://www.emacswiki.org/emacs/GnusGmail#toc1
(setq gnus-select-method '(nntp "news.newsfan.net"))

;; ask encryption password once
(setq epa-file-cache-passphrase-for-symmetric-encryption t)

;; OPTIONAL, the setup for Microsoft Hotmail
(add-to-list 'gnus-secondary-select-methods
             '(nnimap "hotmail"
                      (nnimap-address "imap-mail.outlook.com")
                      (nnimap-server-port 993)
                      (nnimap-stream ssl)
                      (nnir-search-engine imap)
                      (nnmail-expiry-wait 90)))

(setq gnus-thread-sort-functions
      '(gnus-thread-sort-by-most-recent-date
        (not gnus-thread-sort-by-number)))

; NO 'passive
(setq gnus-use-cache t)

;; Fetch only part of the article if we can.
;; I saw this in someone's .gnus
(setq gnus-read-active-file 'some)

;; open attachment
(eval-after-load 'mailcap
  '(progn
     (cond
      ;; on OSX, maybe change mailcap-mime-data?
      ((eq system-type 'darwin))
      ;; on Windows, maybe change mailcap-mime-data?
      ((eq system-type 'windows-nt))
      (t
       ;; Linux, read ~/.mailcap
       (mailcap-parse-mailcaps)))))

;; Tree view for groups.
(add-hook 'gnus-group-mode-hook 'gnus-topic-mode)

;; Threads!  I hate reading un-threaded email -- especially mailing
;; lists.  This helps a ton!
(setq gnus-summary-thread-gathering-function 'gnus-gather-threads-by-subject)

;; Also, I prefer to see only the top level message.  If a message has
;; several replies or is part of a thread, only show the first message.
;; `gnus-thread-ignore-subject' will ignore the subject and
;; look at 'In-Reply-To:' and 'References:' headers.
(setq gnus-thread-hide-subtree t)
(setq gnus-thread-ignore-subject t)

;; Personal Information
(setq user-full-name "Zhang Shuaiyi"
      user-mail-address "zhang_syi@hotmail.com")

;; Read HTML mail
;; You need install the command line web browser 'w3m' and Emacs plugin 'w3m'
(setq mm-text-html-renderer 'w3m)

;; Setup to send email through SMTP
(setq message-send-mail-function 'smtpmail-send-it
      smtpmail-smtp-server 	 "smtp-mail.outlook.com"
      smtpmail-stream-type  	 'starttls
      smtpmail-smtp-service 587)
;; http://www.gnu.org/software/emacs/manual/html_node/gnus/_005b9_002e2_005d.html
(setq gnus-use-correct-string-widths nil)

(eval-after-load 'gnus-topic
  '(progn
     (setq gnus-message-archive-group '((format-time-string "sent.%Y")))
     (setq gnus-server-alist '(("archive" nnfolder "archive" (nnfolder-directory "~/Mail/archive")
                                (nnfolder-active-file "~/Mail/archive/active")
                                (nnfolder-get-new-mail nil)
                                (nnfolder-inhibit-expiry t))))

     (setq gnus-topic-topology '(("Gnus" visible)
                                 (("misc" visible))
                                 (("hotmail" visible nil nil))))

     (setq gnus-topic-alist '(("hotmail" ; the key of topic
                               "nnimap+hotmail:Inbox"
                               "nnimap+hotmail:Drafts"
                               "nnimap+hotmail:Sent"
                               "nnimap+hotmail:Junk"
                               "nnimap+hotmail:Deleted")
                              ("gmail" ; the key of topic
                               "INBOX"
                               "[Gmail]/Sent Mail"
                               "[Gmail]/Trash"
                               "Sent Messages"
                               "Drafts")
                              ("misc" ; the key of topic
                               "nnfolder+archive:sent.2015-12"
                               "nnfolder+archive:sent.2016"
                               "nnfolder+archive:sent.2017"
                               "nndraft:drafts")
                              ("Gnus")))))

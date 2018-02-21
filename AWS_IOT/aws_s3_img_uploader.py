# -*- coding: utf-8 -*-
#!/bin/bash
import os
import sys
import boto
import boto.s3.connection
from boto.s3.key import Key
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from configure import pfc_conf
import datetime

class aws_s3_img_uploader:
    AWS_S3_REGION = pfc_conf.PFC_AWS_S3_REGION
    AWS_ACCESS_KEY = pfc_conf.PFC_AWS_S3_ACCESS_KEY
    AWS_SECRET_KEY = pfc_conf.PFC_AWS_S3_SECRET_KEY
    AWS_BUCKET_NAME = pfc_conf.PFC_AWS_S3_BUCKET_NAME
    AWS_S3_DIR_PATH = pfc_conf.PFC_AWS_S3_DIR_PATH

    def __init__(self):
        None

    def upload_to_s3_bucket(self,img_dir):
        try:

            #max size in bytes before uploading in parts. between 1 and 5 GB recommended
            MAX_SIZE = 20 * 1000 * 1000
            #size of parts when uploading in parts
            PART_SIZE = 6 * 1000 * 1000

            conn = boto.s3.connect_to_region(
                self.AWS_S3_REGION,
                aws_access_key_id = self.AWS_ACCESS_KEY,
                aws_secret_access_key = self.AWS_SECRET_KEY,
                calling_format = boto.s3.connection.OrdinaryCallingFormat(),
            )

            bucket = conn.get_bucket(self.AWS_BUCKET_NAME)
            path = self.AWS_S3_DIR_PATH #Directory Under which file should get upload

            uploadFileNames = []
            for (sourceDir, dirname, filename) in os.walk(img_dir):
                uploadFileNames.extend(filename)
                break

            def percent_cb(complete, total):
                sys.stdout.write('.')
                sys.stdout.flush()

            fidx = 0
            for filename in uploadFileNames:
                fidx +=1
                sourcepath = os.path.join(img_dir + filename)
                destpath = os.path.join(self.AWS_S3_DIR_PATH, filename)
                print str(fidx) + ' . Uploading %s to Amazon S3 bucket[%s]-DIR[%s]' % \
                       (sourcepath, bucket, self.AWS_S3_DIR_PATH)

                filesize = os.path.getsize(sourcepath)
                if filesize > MAX_SIZE:
                    # print "multipart upload"
                    mp = bucket.initiate_multipart_upload(destpath)
                    fp = open(sourcepath,'rb')
                    fp_num = 0
                    while (fp.tell() < filesize):
                        fp_num += 1
                        print "uploading part %i" %fp_num
                        mp.upload_part_from_file(fp, fp_num, num_cb=10, size=PART_SIZE)
                    mp.complete_upload()
                else:
                    # print "singlepart upload"
                    k = boto.s3.key.Key(bucket)
                    k.key = destpath
                    k.set_contents_from_filename(sourcepath, num_cb=10)


        except Exception,e:
            print "Error Execute On the upload to S3 Bucket"
            print str(e)

    def delete_old_imgs(self,img_dir=pfc_conf.IMGS_PATH,include_hours=72):

        fidx = 0
        for dirpath, dirnames, filenames in os.walk(img_dir):
            for file in filenames:
                curpath = os.path.join(dirpath,file)
                file_modified = datetime.datetime.fromtimestamp(os.path.getmtime(curpath))
                if datetime.datetime.now() - file_modified > datetime.timedelta(hours=include_hours) :
                    fidx +=1
                    print str(fidx) + ". "+str(curpath) + ' / ' + str(file_modified) + "   removed."
                    os.remove(curpath)





if __name__ == '__main__':
    s3_uploader = aws_s3_img_uploader()
    s3_uploader.upload_to_s3_bucket(img_dir = pfc_conf.IMGS_PATH)
    s3_uploader.delete_old_imgs(img_dir = pfc_conf.IMGS_PATH,include_hours=72)
















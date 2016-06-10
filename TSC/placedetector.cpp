#include "placedetector.h"
#include "imageprocess/imageprocess.h"
#include "bubble/bubbleprocess.h"

#include <QDebug>

PlaceDetector::PlaceDetector()
{
    this->tempwin = 0;
    this->currentBasePoint.id = 0;
    this->previousBasePoint.id = 0;
    this->placeID = 1;
    currentPlace = new Place(this->placeID);
    this->twindow_counter = 1;
    this->imagesPath = "";
}

float PlaceDetector::processImage()
{
    float dissimilarity = 0;
    if(!currentImage.empty())
    {

        Mat hueChannel= ImageProcess::generateChannelImage(currentImage,0,satLower,satUpper,valLower,valUpper);

        Mat hueChannelFiltered;

        cv::medianBlur(hueChannel, hueChannelFiltered,3);

        vector<bubblePoint> hueBubble = bubbleProcess::convertGrayImage2Bub(hueChannelFiltered,focalLengthPixels,180);
        vector<bubblePoint> reducedHueBubble = bubbleProcess::reduceBubble(hueBubble);

        /************************** Perform filtering and obtain resulting mat images ***********************/
        Mat valChannel= ImageProcess::generateChannelImage(currentImage,2,satLower,satUpper,valLower,valUpper);
        /*****************************************************************************************************/

        /*************************** Convert images to bubbles ***********************************************/
        vector<bubblePoint> valBubble = bubbleProcess::convertGrayImage2Bub(valChannel,focalLengthPixels,255);

        /*****************************************************************************************************/


        /***************** Reduce the bubbles ********************************************************/
        vector<bubblePoint> reducedValBubble = bubbleProcess::reduceBubble(valBubble);


        // Calculate statistics
        bubbleStatistics statsVal =  bubbleProcess::calculateBubbleStatistics(reducedValBubble,255);

        //qDebug()<<"Bubble statistics: "<<statsVal.mean<<statsVal.variance;

        currentBasePoint.avgVal = statsVal.mean;
        currentBasePoint.varVal = statsVal.variance;
        currentBasePoint.id = image_counter;
        currentBasePoint.status = 0;

        /*********************** WE CHECK FOR THE UNINFORMATIVENESS OF THE FRAME   *************************/
        if(statsVal.mean <= this->tau_val_mean || statsVal.variance <= this->tau_val_var)
        {
            currentBasePoint.status = 1;

            // If we don't have an initialized window then initialize
            if(!this->tempwin)
            {
                this->tempwin = new TemporalWindow();
                this->tempwin->tau_n = this->tau_n;
                this->tempwin->tau_w = this->tau_w;
                this->tempwin->startPoint = image_counter;
                this->tempwin->endPoint = image_counter;
                this->tempwin->id = twindow_counter;
                this->tempwin->totalDiff = 0.99;
                this->tempwin->members.push_back(currentBasePoint);
            }
            else
            {
                this->tempwin->endPoint = image_counter;
                this->tempwin->totalDiff += 0.99;
                this->tempwin->members.push_back(currentBasePoint);
            }

            ///  dbmanager.insertBasePoint(currentBasePoint);
            wholebasepoints.push_back(currentBasePoint);

            image_counter++;

            this->currentImage.release();

            this->shouldProcess = true;

            return dissimilarity;
        }
        /***********************************  IF THE FRAME IS INFORMATIVE *************************************************/
        else
        {
            Mat totalInvariants;

            DFCoefficients dfcoeff = bubbleProcess::calculateDFCoefficients(reducedHueBubble,noHarmonics,noHarmonics);
            Mat hueInvariants = bubbleProcess::calculateInvariantsMat(dfcoeff,noHarmonics, noHarmonics);

            totalInvariants = hueInvariants.clone();

            cv::Mat logTotal;

            Mat grayImage;

            cv::cvtColor(currentImage,grayImage,CV_BGR2GRAY);

            std::vector<Mat> sonuc = ImageProcess::applyFilters(grayImage);


            for(uint j = 0; j < sonuc.size(); j++)
            {
                vector<bubblePoint> imgBubble = bubbleProcess::convertGrayImage2Bub(sonuc[j],focalLengthPixels,255);

                vector<bubblePoint> resred = bubbleProcess::reduceBubble(imgBubble);

                DFCoefficients dfcoeff =  bubbleProcess::calculateDFCoefficients(resred,noHarmonics,noHarmonics);

                Mat invariants=  bubbleProcess::calculateInvariantsMat(dfcoeff,noHarmonics,noHarmonics);

//                if(j==0)
//                    totalInvariants = invariants.clone();
//                else
//                    cv::hconcat(totalInvariants, invariants, totalInvariants);
                cv::hconcat(totalInvariants, invariants, totalInvariants);
            }

            cv::log(totalInvariants,logTotal);
            logTotal = logTotal/25;
            cv::transpose(logTotal,logTotal);

            // TOTAL INVARIANTS N X 1 vector
            for(int kk = 0; kk < logTotal.rows; kk++)
            {
                if(logTotal.at<float>(kk,0) < 0)
                    logTotal.at<float>(kk,0) = 0.5;
            }

            // We don't have a previous base point
            if(previousBasePoint.id == 0)
            {
                currentBasePoint.id = image_counter;
                currentBasePoint.invariants = logTotal;
                previousBasePoint = currentBasePoint;

                currentPlace->members.push_back(currentBasePoint);

                /// dbmanager.insertBasePoint(currentBasePoint);
                wholebasepoints.push_back(currentBasePoint);

            }
            else
            {
                currentBasePoint.id = image_counter;
                currentBasePoint.invariants = logTotal;

                // MY VERSION FOR CHISQR, SIMPLER!!
                double result= compareHKCHISQR(currentBasePoint.invariants,previousBasePoint.invariants);
                dissimilarity = result;
                //qDebug()<<"Invariant diff between "<<currentBasePoint.id<<previousBasePoint.id<<"is"<<result;

                ///////////////////////////// IF THE FRAMES ARE COHERENT ///////////////////////////////////////////////////////////////////////////////////////////////////////
                if(result <= tau_inv && result > 0)
                {
                    ///  dbmanager.insertBasePoint(currentBasePoint);
                    wholebasepoints.push_back(currentBasePoint);

                    /// If we have a temporal window
                    if(tempwin)
                    {
                        // Temporal window will extend, we are still looking for the next incoming frames
                        if(tempwin->checkExtensionStatus(currentBasePoint.id))
                        {
                            tempwin->cohMembers.push_back(currentBasePoint);
                            basepointReservoir.push_back(currentBasePoint);
                        }
                        // Temporal window will not extend anymore, we should check whether it is really a temporal window or not
                        else
                        {
                            float area = this->tempwin->totalDiff/(tempwin->endPoint - tempwin->startPoint+1);

                            //qDebug()<<"Temporal Window Area"<<area;
                            //qDebug()<<"Temporal Window Start End"<<tempwin->startPoint<<tempwin->endPoint;

                            // This is a valid temporal window
                            if(tempwin->endPoint - tempwin->startPoint >= tau_w && area>= tau_avgdiff)
                            {
                                qDebug()<<"New Place";
                                currentPlace->calculateMeanInvariant();

                                qDebug()<<"Current place mean invariant: "<<currentPlace->meanInvariant.rows<<currentPlace->meanInvariant.cols<<currentPlace->meanInvariant.at<float>(50,0);

                                if(currentPlace->memberIds.rows >= tau_p){

                                    dbmanager->insertPlace(*currentPlace);

                                    this->detectedPlaces.push_back(*currentPlace);

                                    this->placeID++;
                                }

                                delete currentPlace;
                                currentPlace = 0;

                                currentPlace = new Place(this->placeID);

                                basepointReservoir.push_back(currentBasePoint);

                                currentPlace->members = basepointReservoir;
                                basepointReservoir.clear();

                                dbmanager->insertTemporalWindow(*tempwin);

                                delete tempwin;
                                tempwin = 0;
                                this->twindow_counter++;
                                // A new place will be created. Current place will be published
                            }
                            // This is just a noisy temporal window. We should add the coherent basepoints to the current place
                            else
                            {
                                basepointReservoir.push_back(currentBasePoint);

                                delete tempwin;
                                tempwin = 0;

                                std::vector<BasePoint> AB;
                                AB.reserve( currentPlace->members.size() + basepointReservoir.size() ); // preallocate memory
                                AB.insert( AB.end(), currentPlace->members.begin(), currentPlace->members.end() );
                                AB.insert( AB.end(), basepointReservoir.begin(), basepointReservoir.end() );
                                currentPlace->members.clear();
                                currentPlace->members = AB;

                                basepointReservoir.clear();
                            }
                        }
                    }
                    else
                    {
                        currentPlace->members.push_back(currentBasePoint);
                    }
                }
                ///////////////////////// IF THE FRAMES ARE INCOHERENT /////////////////////////////////////
                else
                {
                    currentBasePoint.status = 2;

                    ///    dbmanager.insertBasePoint(currentBasePoint);
                    wholebasepoints.push_back(currentBasePoint);



                    // If we don't have a temporal window create one
                    if(!tempwin)
                    {
                        tempwin = new TemporalWindow();
                        this->tempwin->tau_n = this->tau_n;
                        this->tempwin->tau_w = this->tau_w;
                        this->tempwin->startPoint = image_counter;
                        this->tempwin->endPoint = image_counter;
                        this->tempwin->id = twindow_counter;
                        this->tempwin->totalDiff +=result;
                        this->tempwin->members.push_back(currentBasePoint);

                    }
                    // add the basepoint to the temporal window
                    else
                    {
                        // Temporal window will extend, we are still looking for the next incoming frames
                        if(tempwin->checkExtensionStatus(currentBasePoint.id))
                        {

                            this->tempwin->endPoint = image_counter;

                            this->tempwin->members.push_back(currentBasePoint);

                            this->tempwin->totalDiff +=result;

                            basepointReservoir.clear();


                        }
                        else
                        {
                            float avgdiff;

                            avgdiff = this->tempwin->totalDiff/(tempwin->endPoint - tempwin->startPoint+1);


                            //qDebug()<<"Temporal Window Average Diff"<<avgdiff;
                            //qDebug()<<"Temporal Window Start End"<<tempwin->startPoint<<tempwin->endPoint;

                            // This is a valid temporal window
                            if(tempwin->endPoint - tempwin->startPoint >= tau_w && avgdiff >= tau_avgdiff)
                            {
                                qDebug()<<"New Place";
                                currentPlace->calculateMeanInvariant();

                                qDebug()<<"Current place mean invariant: "<<currentPlace->meanInvariant.rows<<currentPlace->meanInvariant.cols<<currentPlace->meanInvariant.at<float>(50,0);

                                if(currentPlace->memberIds.rows >= tau_p)
                                {

                                    dbmanager->insertPlace(*currentPlace);

                                    this->detectedPlaces.push_back(*currentPlace);

                                    this->placeID++;
                                }


                                delete currentPlace;
                                currentPlace = 0;
                                currentPlace = new Place(this->placeID);

                                currentPlace->members = basepointReservoir;
                                basepointReservoir.clear();

                                dbmanager->insertTemporalWindow(*tempwin);

                                delete tempwin;
                                tempwin = 0;
                                this->twindow_counter++;
                                // A new place will be created. Current place will be published

                            }
                            // This is just a noisy temporal window. We should add the coherent basepoints to the current place
                            else
                            {
                                delete tempwin;
                                tempwin = 0;

                                std::vector<BasePoint> AB;
                                AB.reserve( currentPlace->members.size() + basepointReservoir.size() ); // preallocate memory
                                AB.insert( AB.end(), currentPlace->members.begin(), currentPlace->members.end() );
                                AB.insert( AB.end(), basepointReservoir.begin(), basepointReservoir.end() );
                                currentPlace->members.clear();
                                currentPlace->members = AB;

                                basepointReservoir.clear();
                            }

                            tempwin = new TemporalWindow();
                            this->tempwin->tau_n = this->tau_n;
                            this->tempwin->tau_w = this->tau_w;
                            this->tempwin->startPoint = image_counter;
                            this->tempwin->endPoint = image_counter;
                            this->tempwin->id = twindow_counter;

                            this->tempwin->members.push_back(currentBasePoint);
                        }
                    }
                }
                previousBasePoint = currentBasePoint;
            }
            image_counter++;
        }
    }

    this->currentImage.release();

    this->shouldProcess = true;

    return dissimilarity;
}

double PlaceDetector::compareHKCHISQR(Mat input1, Mat input2)
{
    double res = -1;

    if(input1.rows != input2.rows)
    {
        qDebug()<<"Comparison failed due to col size mismatch";
        return res;
    }
    double summ  = 0;
    for(int i = 0; i < input1.rows; i++)
    {
        float in1 = input1.at<float>(i,0);
        float in2 = input2.at<float>(i,0);

        double mul = (in1-in2)*(in1-in2);

        double ss =  in1+in2;
        summ += mul/ss;
    }
    return summ;
}


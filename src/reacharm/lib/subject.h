/**
 * \file	subject.h
 * \author	Thibaut Mattio <thibaut.mattio@gmail.com>
 * \author  Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
 * \author  Karl Ritchie <ritchie.karl@gmail.com>
 * \author  Mathieu Benoit <mathben963@gmail.com>
 * \date	03/10/2015
 */

#ifndef REACHARM_LIB_SUBJECT_H_
#define REACHARM_LIB_SUBJECT_H_

#include <vector>
#include <mutex>

template <typename... Args_>
class Observer;

/**
 * A subject is an object that will send notification when is internal state
 * have changed.
 *
 * Object inheriting from Observer<Args_...> can be attached to the subject.
 * They will then receive the notifications that are sent from the subject.
 *
 * When inhering from a subject, the derived class can specify a list of
 * parameter it wants to send while sending a notifycation. Thus, the observer
 * that wants to subscibe to this derived subject will have to implement a
 * method providing the same parameters.
 *
 * Sample usage:
 *
 * public class ImageProvider : public atlas::Subject<const cv::Mat &> {
 *  public:
 *   void SearchForImage() {
 *     while (!must_stop_searching_) {
 *       std::shared_ptr<cv::Mat> image =
 *           GetImageFromDirectory("~/.image_dir");
 *       if (image != nullptr) {
 *         Notify(*image);
 *       }
 *     }
 *   }
 *  private:
 *   bool must_stop_searching_ = {false};
 * };
 *
 * \template Args_ A list of arguments to send when a notification is thown.
 * This will usually be the list of the member an observer wants to access --
 * e.g. A reference to an image if the subject is an image provider
 */
template <typename... Args_>
class Subject {
  // For optimization purpose, we want the observer to call the detach from
  // the subject without calling a OnSubjectDisconnected after.
  // In this case, we created a DetachNoCallback private function that can be
  // called only from the Observer or internally.
  friend class Observer<Args_...>;

 public:
  //============================================================================
  // P U B L I C   C / D T O R S

  Subject() noexcept;

  virtual ~Subject() noexcept;

  explicit Subject(const Subject<Args_...> &rhs) noexcept;

  //============================================================================
  // P U B L I C   O P E R A T O R S

  void operator=(const Subject<Args_...> &) noexcept;

  //============================================================================
  // P U B L I C  M E T H O D S

  /**
   * Throw a notification to all the observers that have attached to this
   * subject.
   *
   * \param args The arguments that
   */
  void Notify(Args_... args) noexcept;

  /**
   * Return the number of observers attached to this subject.
   */
  size_t ObserverCount() const noexcept;

  /**
   * Add a new observer to the list. Return false if already in the attached.
   */
  void Attach(Observer<Args_...> &observer);

  /**
   * Remove an observer from the list. Return false if it was not attached.
   */
  void Detach(Observer<Args_...> &observer);

  /**
   * Remove an observer from the list. Return false if it was not attached.
   */
  void DetachAll() noexcept;

 private:
  //============================================================================
  // P R I V A T E   M E T H O D S

  /**
   * Detach an observer without calling a callback.
   *
   * This provides the same functionality as DetachObserver but guarantees not
   * to call back the observer being disconnect (as is done in DetachObserver).
   * This is used strictly by ObserverBase during its destruction.
   *
   * Throws an invalid_argument arguments exception if the observer passed in
   * argument is not attached to this subject.
   *
   * \param observer The observer we want to detach from this subject.
   */
  void DetachNoCallback(Observer<Args_...> &observer);

  //============================================================================
  // P R I V A T E   M E M B E R S

  /**
   * This list of all the observers that are currently attached to this subject.
   */
  std::vector<Observer<Args_...> *> observers_;

  /**
   * A mutex that is going to be locked every time we try to access the list
   * of observers.
   * This provides thread safety for the all class ressources.
   */
  mutable std::mutex observers_mutex_;
};

#include <reacharm/lib/subject_inl.h>

#endif  // REACHARM_LIB_SUBJECT_H_
